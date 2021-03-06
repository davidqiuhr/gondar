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

#ifndef SRC_UPDATE_CHECK_H_
#define SRC_UPDATE_CHECK_H_

#include <QObject>

class QNetworkReply;

namespace gondar {

class UpdateCheck : public QObject {
  Q_OBJECT

 public:
  void start(QWidget* parent);

 private:
  void handleReply(QNetworkReply* reply);
  void showMessage(const QString& version);

  // Parent widget for the "update-now" dialog
  QWidget* parent_ = nullptr;
};
}  // namespace gondar

#endif  // SRC_UPDATE_CHECK_H_
