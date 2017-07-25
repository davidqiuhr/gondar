// Copyright 2017 Neverware

#ifndef SRC_MEEPO_H_
#define SRC_MEEPO_H_

#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

  QString error() const;
  Sites sites() const;

 signals:
  void finished();

 private:
  void requestAuth(const QAuthenticator& auth);
  void handleAuthReply(QNetworkReply* reply);

  void requestSites();
  void handleSitesReply(QNetworkReply* reply);

  void requestDownloads(const GondarSite& site);
  void handleDownloadsReply(QNetworkReply* reply);

  void dispatchReply(QNetworkReply* reply);
  void fail(const QString& error);

  GondarSite* siteFromSiteId(const int site_id);

  QNetworkAccessManager network_manager_;

  QString api_token_;
  Sites sites_;
  QString error_;
  int sites_remaining_ = 0;
};

}  // namespace gondar

#endif  // SRC_MEEPO_H_
