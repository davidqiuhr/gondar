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
#include "log.h"
#include "oauth_server.h"
#include "util.h"

// TODO(kendall): right now we are just using current time as rand seed
static QString get_random_string() {
  // most basic version first: all lowercase characters
  const QString possibleCharacters(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  auto len = 20;
  QString ret;
  gondar::maybeInitRand();
  for (auto i = 0; i < len; i++) {
    auto index = std::rand() % possibleCharacters.length();
    QChar cur_char = possibleCharacters.at(index);
    ret.append(cur_char);
  }
  return ret;
}

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
  meanWordsLabel.setText("That's not right!  Try again.");
  QString forgotText = "<a href=\"https://my." + gondar::getDomain() +
                       "/forgot-password\">Forgot your password?</a>";
  forgotLabel.setText(forgotText);
  forgotLabel.setTextFormat(Qt::RichText);
  forgotLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);
  forgotLabel.setOpenExternalLinks(true);
  client_id = gondar::getGoogleSignInId();
  client_secret = gondar::getGoogleSignInSecret();
  redirect_uri = QString("http://127.0.0.1:%1").arg(gondar::getPort());
  state = get_random_string();
  code_verifier = get_random_string();
  googleLabel.setText("<a href='#'>Sign in with Google</a>");
  googleLabel.setTextFormat(Qt::RichText);
  googleLabel.setTextInteractionFlags(Qt::TextBrowserInteraction);
  meanWordsLabel.setObjectName("loginError");
  layout.addWidget(&passwordLineEditLabel, 1, 0);
  layout.addWidget(&passwordLineEdit, 1, 1);
  layout.addWidget(&meanWordsLabel, 2, 0, 1, 2);
  layout.addWidget(&forgotLabel, 3, 0, 1, 2);
  layout.addWidget(&googleLabel, 4, 0, 1, 2);
  meanWordsLabel.setVisible(false);
  // if our build does not have a client id and client secret configured,
  // do not display the 'sign in with google' text
  if (client_id.isEmpty() || client_secret.isEmpty()) {
    googleLabel.setVisible(false);
  }
  setLayout(&layout);
  // don't allow progressing to next page yet
  finished = false;
  // don't allow another launch of server interaction while another is running
  started = false;
  // connect our various event listeners
  connect(&googleLabel, &QLabel::linkActivated, this,
          &ChromeoverLoginPage::handleGoogleSigninPart1);
  connect(&localServer, &OauthServer::authError, this,
          &ChromeoverLoginPage::handleGoogleSigninError);
  connect(&localServer, &OauthServer::callbackReceived, this,
          &ChromeoverLoginPage::handleGoogleSigninPart2);
  connect(&manager, &QNetworkAccessManager::finished, this,
          &ChromeoverLoginPage::handleGoogleSigninPart3);
  connect(&meepo_, &gondar::Meepo::finished, this,
          &ChromeoverLoginPage::handleMeepoFinished);
}

void ChromeoverLoginPage::handleGoogleSigninError(QString error) {
  LOG_WARNING << "received google sign in error=" << error;
}

// This fires when the user has clicked on the 'sign in with google' link
// we generate the URL to open in their browser, send them there, and start
// the local server to parse the sign-in response
void ChromeoverLoginPage::handleGoogleSigninPart1() {
  static bool http_server_started = false;
  if (http_server_started == false) {
    http_server_started = true;
    localServer.start();
    QString googleText =
        QString(
            "https://accounts.google.com/o/oauth2/v2/"
            "auth?client_id=%1&redirect_uri=%2&response_type=code&scope=email&"
            "state=%3&code_"
            "challenge_method=plain&code_challenge=%4")
            .arg(client_id, redirect_uri, state, code_verifier);
    // notably, using QDesktopServices instead of using a regular hyperlink
    // appears to suppress the windows warning about opening a local port
    QDesktopServices::openUrl(QUrl(googleText));
  }
}

// This fires when the local server has received the code required to receive
// a token.  We send a request to the token url, passing along this code
// and some other identification information to get our token.
void ChromeoverLoginPage::handleGoogleSigninPart2(QString state_in,
                                                  QString code) {
  // make sure we get back the same state string as was sent in initial request
  if (state_in == state) {
    LOG_INFO << "Received 'Sign-in with Google' response to local server";
    QJsonObject json;
    json["code"] = code;
    json["client_id"] = client_id;
    json["client_secret"] = client_secret;
    json["redirect_uri"] = redirect_uri;
    json["grant_type"] = "authorization_code";
    json["code_verifier"] = code_verifier;
    QJsonDocument doc(json);
    QUrl tokenUrl = QUrl("https://www.googleapis.com/oauth2/v4/token");
    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    manager.post(request, doc.toJson(QJsonDocument::Compact));
  } else {
    // TODO(kendall): explicitly show "that's not right..." text to user?
    LOG_WARNING << "error: state did not match";
  }
}

// This fires when we have received our token. We send the token to Meepo
// to find the user's sites in a way very similar to user/password flow.
// TODO(kendall): handle failure case
void ChromeoverLoginPage::handleGoogleSigninPart3(QNetworkReply* reply) {
  LOG_INFO << "id token received; contacting meepo...";
  QString id_token = gondar::jsonFromReply(reply)["id_token"].toString();
  if (!started) {
    meepo_.startGoogle(id_token);
    started = true;
  }
}

int ChromeoverLoginPage::nextId() const {
  const auto& siteList = wizard()->sites();
  if (siteList.size() == 0) {
    return GondarWizard::Page_error;
  } else if (siteList.size() > 1) {
    // if there are multiple sites, we'll set the urls on the site select page
    return GondarWizard::Page_siteSelect;
  } else {
    wizard()->imageSelectPage.addImages(siteList[0].getImages());
    return GondarWizard::Page_imageSelect;
  }
}

bool ChromeoverLoginPage::validatePage() {
  QAuthenticator auth;
  auth.setUser(usernameLineEdit.text());
  auth.setPassword(passwordLineEdit.text());
  if (finished) {
    localServer.stop();
    return true;
  }
  if (!started) {
    meepo_.start(auth);
  }
  started = true;
  return false;
}

void ChromeoverLoginPage::handleMeepoFinished() {
  if (meepo_.error().isEmpty()) {
    wizard()->setSites(meepo_.sites());

    // we don't want users to be able to pass through the screen by pressing
    // next while processing.  this will make validatePage pass and immediately
    // move the user on to the next screen
    finished = true;
    // TODO(kendall): let's make this a commit page so we don't have to worry
    // about the case of a user revisiting this segment for now
    wizard()->next();
    // If the user has no sites, proceed to the error screen with the
    // appropriate error
  } else if (meepo_.no_sites_error == meepo_.error()) {
    finished = true;
    wizard()->postError(meepo_.no_sites_error);
    // otherwise, assume login credentials are incorrect and prompt them to
    // retry
  } else {
    started = false;
    meanWordsLabel.setVisible(true);
  }
}
