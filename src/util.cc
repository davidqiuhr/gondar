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

#include "util.h"

#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>

#include "config.h"
#include "log.h"

namespace gondar {

QString readUtf8File(const QString& filepath) {
  LOG_INFO << "opening " << filepath << " for reading";
  QFile file(filepath);
  if (!file.open(QFile::ReadOnly)) {
    throw std::runtime_error("error opening file for reading: " +
                             filepath.toStdString());
  }
  return QString::fromUtf8(file.readAll());
}

// helper function to determine if this build is a chromeover build
bool isChromeover() {
#ifdef CHROMEOVER
  return true;
#else
  return false;
#endif
}

bool isRelease() {
#ifdef RELEASE
  return true;
#else
  return false;
#endif
}

QString getDomain() {
  if (gondar::isRelease()) {
    LOG_INFO << "Using release licensing endpoint";
    return QString("neverware.com");
  } else {
    LOG_INFO << "Using dev licensing endpoint";
    return QString("grv.neverware.com");
  }
}

QString getGondarVersion() {
  return QString("2.2");
}

QJsonObject jsonFromReply(QNetworkReply* reply) {
  QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
  return jsonDoc.object();
}

// the number of bytes in a gigabyte (2**30)
uint64_t getGigabyte() {
  return 1073741824LL;
}
}  // namespace gondar
