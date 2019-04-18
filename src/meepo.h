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

#ifndef SRC_MEEPO_H_
#define SRC_MEEPO_H_

#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

#include "gondarsite.h"

namespace gondar {

class Meepo : public QObject {
  Q_OBJECT

 public:
  using Sites = std::vector<GondarSite>;

  Meepo();

  // Currently we only use the Meepo API one way: log in, get the list
  // of sites for that user, then get the download links for those
  // sites. Call this method to initiate that flow, and await the
  // finished() signal to get the results (including errors).
  void start(const QAuthenticator& auth);
  void startGoogle(const QString id_token);

  QString error() const;
  Sites sites() const;
  const QString no_sites_error = "User has no sites";

 signals:
  void finished();
  void failed(bool google_mode);

 private:
  void requestAuth(const QAuthenticator& auth);
  void requestGoogleAuth(QString);
  void handleAuthReply(QNetworkReply* reply);

  void requestSites();
  void handleSitesReply(QNetworkReply* reply);

  void requestDownloads(const GondarSite& site);
  void handleDownloadsReply(QNetworkReply* reply);

  void dispatchReply(QNetworkReply* reply);
  void fail(const QString& error);

  // used by requestAuth() and requestGoogleAuth()
  void clear();

  GondarSite* siteFromSiteId(const int site_id);

  QNetworkAccessManager network_manager_;

  QString api_token_;
  Sites sites_;
  QString error_;
  int sites_remaining_ = 0;
  // whether or not the user is currently authenticating using
  // 'sign in with google'
  bool google_mode_ = false;
};

}  // namespace gondar

#endif  // SRC_MEEPO_H_
