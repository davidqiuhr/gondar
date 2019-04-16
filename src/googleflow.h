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

#ifndef SRC_GOOGLEFLOW_H_
#define SRC_GOOGLEFLOW_H_

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

#include "oauth_server.h"

class QNetworkReply;

class GoogleFlow : public QObject {
  Q_OBJECT

 public:
  GoogleFlow();
  void stopServer();  // stop the local webserver
  bool shouldShowSignInWithGoogle();
  QNetworkAccessManager* getManager();
 public slots:
  void handleGoogleSigninPart1();

 signals:
  void errorMiddle();

 private:
  void handleGoogleSigninError(QString error);
  void handleGoogleSigninPart2(QString state_in, QString code);
  OauthServer localServer;
  QString client_id;
  QString state;
  QString client_secret;
  QString redirect_uri;
  QString code_verifier;
  QNetworkAccessManager manager;
  bool http_server_started = false;
};
#endif  // SRC_GOOGLEFLOW_H_
