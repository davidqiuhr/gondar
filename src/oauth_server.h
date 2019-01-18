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

#ifndef SRC_OAUTH_SERVER_H_
#define SRC_OAUTH_SERVER_H_

#include <QObject>
#include <QString>

#include <microhttpd.h>

class OauthServer : public QObject {
  Q_OBJECT
 public:
  explicit OauthServer(QObject* parent = 0);
  void start();
  void stop();
 signals:
  void callbackReceived(QString state, QString code);
  void authError(QString error);

 private:
  struct MHD_Daemon* daemon;
};

#endif  // SRC_OAUTH_SERVER_H_
