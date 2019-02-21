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

#include <random>

#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QString>

#include "config.h"
#include "log.h"
#include "xor.h"

namespace gondar {

// get the google sign in client id
// see https://console.developers.google.com
QByteArray getGoogleSignInId() {
#ifdef GOOGLE_SIGN_IN_CLIENT
  return QByteArray(GOOGLE_SIGN_IN_CLIENT);
#else
  return QByteArray();
#endif
}

// get the google sign in client secret
QByteArray getGoogleSignInSecret() {
// TODO(kendall): ifdef on both hash1 and 2
#ifdef GOOGLE_SIGN_IN_SECRET_HASH1
  QString client_secret = get_string_from_hashes(GOOGLE_SIGN_IN_SECRET_HASH1,
                                                 GOOGLE_SIGN_IN_SECRET_HASH2);
  LOG_WARNING << "hash1=" << GOOGLE_SIGN_IN_SECRET_HASH1;
  LOG_WARNING << "hash2=" << GOOGLE_SIGN_IN_SECRET_HASH2;
  LOG_WARNING << "secret=" << client_secret.toLatin1();
  return client_secret.toLatin1();
#else
  return QByteArray();
#endif
}

// the range is inclusive according to
// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
// I believe a seed is generated on each call this way, but we only call it
// twice so that should be fine.
int getRandomNum(int lower, int higher) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(lower, higher);
  int result = dis(gen);
  // FIXME(kendall): i appear to be generating one good number, then 34 over
  // and over.  i should probably save my seed instead of calling gen over
  // and over
  LOG_INFO << "generated " << result;
  return result;
}

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
  return QString("");
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
