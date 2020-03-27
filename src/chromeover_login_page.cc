// Copyright 2017 Neverware
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "chromeover_login_page.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QUrl>
#include <cstdlib>
#include <ctime>

#include "gondarsite.h"
#include "gondarwizard.h"
#include "googleflow.h"
#include "log.h"
#include "metric.h"
#include "oauth_server.h"
#include "util.h"

ChromeoverLoginPage::ChromeoverLoginPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Login");
  setSubTitle("Enter credentials for my.neverware.com.");
  // configure username fields
  usernameLineEditLabel.setBuddy(&usernameLineEdit);
  usernameLineEditLabel.setText("Email:");
  layout.addWidget(&usernameLineEditLabel, 0, 0);
  layout.addWidget(&usernameLineEdit, 0, 1);
  // configure password fields
  passwordLineEdit.setEchoMode(QLineEdit::Password);
  passwordLineEditLabel.setBuddy(&passwordLineEdit);
  passwordLineEditLabel.setText("Password:");
  QString forgotText = "<a href=\"https://my." + gondar::getDomain() +
                       "/forgot-password\">Forgot your password?</a>";
  forgotLabel.setText(forgotText);
  forgotLabel.setTextFormat(Qt::RichText);
  forgotLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);
  forgotLabel.setOpenExternalLinks(true);

  // image sourced from
  // https://developers.google.com/identity/branding-guidelines
  googleButton.setObjectName("googleSigninButton");

  meanWordsLabel.setObjectName("loginError");
  layout.addWidget(&passwordLineEditLabel, 1, 0);
  layout.addWidget(&passwordLineEdit, 1, 1);
  layout.addWidget(&meanWordsLabel, 2, 0, 1, 2);
  layout.addWidget(&forgotLabel, 3, 0, 1, 2);
  layout.addWidget(&googleButton, 4, 0, 1, 2);
  meanWordsLabel.setVisible(false);
  // if our build does not have a client id and client secret configured,
  // do not display the 'sign in with google' text
  googleButton.setVisible(googleFlow.shouldShowSignInWithGoogle());
  setLayout(&layout);
  // don't allow progressing to next page yet
  finished = false;
  // don't allow another launch of server interaction while another is running
  started = false;
  // connect events between our members and ourself
  connect(&googleButton, &QPushButton::clicked, &googleFlow,
          &GoogleFlow::handleGoogleSigninPart1);
  connect(googleFlow.getManager(), &QNetworkAccessManager::finished, this,
          &ChromeoverLoginPage::handleGoogleSigninFinished);
  connect(&googleFlow, &GoogleFlow::errorMiddle, this,
          &ChromeoverLoginPage::handleGoogleSigninFail);

  // signals for button rendering
  connect(&googleButton, &QPushButton::pressed, this,
          &ChromeoverLoginPage::showPressedButton);
  connect(&googleButton, &QPushButton::released, this,
          &ChromeoverLoginPage::showUnpressedButton);
}

void ChromeoverLoginPage::showPressedButton() {
  setStyleSheet(
      "#googleSigninButton { background-image: "
      "url(':/images/btn_google_signin_light_pressed_web.png') }");
}
void ChromeoverLoginPage::showUnpressedButton() {
  setStyleSheet(
      "#googleSigninButton { background-image: "
      "url(':/images/btn_google_signin_light_normal_web.png') }");
  setStyleSheet(
      "#googleSigninButton:hover { background-image: "
      "url(':/images/btn_google_signin_light_focus_web.png') }");
}

int ChromeoverLoginPage::nextId() const {
  const auto& siteList = wizard()->sites();
  if (siteList.size() == 0) {
    return GondarWizard::Page_error;
  } else if (siteList.size() > 1) {
    // if there are multiple sites, we'll set the urls on the site select page
    return GondarWizard::Page_siteSelect;
  } else {
    // populate the current site for metrics now as we'll be skipping the
    // site select page; OVER-11255
    gondar::SetSiteId(siteList[0].getSiteId());
    wizard()->imageSelectPage.addImages(siteList[0].getImages());
    return GondarWizard::Page_imageSelect;
  }
}

bool ChromeoverLoginPage::validatePage() {
  QAuthenticator auth;
  auth.setUser(usernameLineEdit.text());
  auth.setPassword(passwordLineEdit.text());
  if (finished) {
    googleFlow.stopServer();
    return true;
  }
  if (!started) {
    wizard()->meepo_.start(auth);
  }
  started = true;
  return false;
}

void ChromeoverLoginPage::handleMeepoFinished() {
  wizard()->setSites(wizard()->meepo_.sites());

  // we don't want users to be able to pass through the screen by pressing
  // next while processing.  this will make validatePage pass and immediately
  // move the user on to the next screen
  finished = true;
  // TODO(kendall): let's make this a commit page so we don't have to worry
  // about the case of a user revisiting this segment for now
  wizard()->next();
}

void ChromeoverLoginPage::handleMeepoFailed(bool using_google) {
  // If the user has no sites, proceed to the error screen with the
  // appropriate error
  if (wizard()->meepo_.no_sites_error == wizard()->meepo_.error()) {
    finished = true;
    wizard()->postError(wizard()->meepo_.no_sites_error);
    // otherwise, assume login credentials are incorrect and prompt them to
    // retry
  } else {
    started = false;
    if (using_google) {
      meanWordsLabel.setText("That Google user doesn't have an account.");
    } else {
      meanWordsLabel.setText("That's not right!  Try again.");
    }
    meanWordsLabel.setVisible(true);
  }
}

// This fires when we have received our token. We send the token to Meepo
// to find the user's sites in a way very similar to user/password flow.
void ChromeoverLoginPage::handleGoogleSigninFinished(QNetworkReply* reply) {
  LOG_INFO << "id token received; contacting meepo...";
  // remove red error text that may have existed from a previous auth failure
  meanWordsLabel.setVisible(false);
  QString id_token = gondar::jsonFromReply(reply)["id_token"].toString();
  wizard()->setToken(id_token);
  if (!started) {
    wizard()->meepo_.startGoogle(id_token);
    started = true;
  }
}

void ChromeoverLoginPage::handleGoogleSigninFail() {
  LOG_WARNING << "Sign in with Google failed; resetting";
  // we show the red text
  meanWordsLabel.setText("Authentication error signing in with Google");
  meanWordsLabel.setVisible(true);
  // then we turn off the server so we can repeat the flow similarly to
  // the initial run
  googleFlow.stopServer();
}
