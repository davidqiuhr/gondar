// Copyright 2019 Neverware
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

// use possibleCharacters to generate a random string
// based on:
// https://stackoverflow.com/questions/18862963/qt-c-random-string-generation

#include "googleflow.h"

#include <QDesktopServices>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QString>
#include <QUrl>

#include "log.h"
#include "rand_util.h"
#include "util.h"

static QString get_random_string() {
  const QString possibleCharacters(
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  auto len = 20;
  QString ret;
  for (auto i = 0; i < len; i++) {
    auto index = gondar::getRandomNum(0, possibleCharacters.length() - 1);
    QChar cur_char = possibleCharacters.at(index);
    ret.append(cur_char);
  }
  return ret;
}

GoogleFlow::GoogleFlow() {
  client_id = gondar::getGoogleSignInId();
  client_secret = gondar::getGoogleSignInSecret();
  redirect_uri = QString("http://127.0.0.1:%1").arg(localServer.getPort());
  state = get_random_string();
  code_verifier = get_random_string();
  // connect our various event listeners
  connect(&localServer, &OauthServer::authError, this,
          &GoogleFlow::handleGoogleSigninError);
  connect(&localServer, &OauthServer::callbackReceived, this,
          &GoogleFlow::handleGoogleSigninPart2);
}

// this function is just a middleman so that the page does not have to know
// about oauth server
void GoogleFlow::handleGoogleSigninError(QString error) {
  LOG_WARNING << "received google sign in error=" << error;
  emit errorMiddle();
}

// This fires when the user has clicked on the 'sign in with google' link
// we generate the URL to open in their browser, send them there, and start
// the local server to parse the sign-in response
void GoogleFlow::handleGoogleSigninPart1() {
  if (http_server_started == false) {
    http_server_started = true;
    localServer.start();
  }
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

// This fires when the local server has received the code required to receive
// a token.  We send a request to the token url, passing along this code
// and some other identification information to get our token.
void GoogleFlow::handleGoogleSigninPart2(QString state_in, QString code) {
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
// TODO(ken): consider how we can best correlate these metrics with meepo
// metrics, ideally without handling an email address locally

bool GoogleFlow::shouldShowSignInWithGoogle() {
  return !client_id.isEmpty() && !client_secret.isEmpty();
}

QNetworkAccessManager* GoogleFlow::getManager() {
  return &manager;
}

void GoogleFlow::stopServer() {
  localServer.stop();
  http_server_started = false;
}
