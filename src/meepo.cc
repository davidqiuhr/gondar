// Copyright 2017 Neverware

#include "meepo.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "gondarsite.h"
#include "log.h"
#include "networksingleton.h"

namespace {

const char path_auth[] = "/auth";
const char path_sites[] = "/sites";
const char path_downloads[] = "/downloads";

// TODO(nicholasbishop): this is hardcoded to the staging URL for now
QUrl createUrl(const QString& path) {
  return QUrl("https://api.grv.neverware.com/poof" + path);
}

QString redactedUrl(QUrl url) {
  QUrlQuery query(url);
  query.removeQueryItem("password");
  query.removeQueryItem("token");
  url.setQuery(query);
  return url.toString();
}

int siteIdFromUrl(const QUrl& url) {
  const auto path = url.path();
  const auto parts = path.split('/');
  const auto sites_index = parts.lastIndexOf("sites");

  if (sites_index == -1) {
    LOG_ERROR << "failed to find 'sites' in " << redactedUrl(url);
    return -1;
  }

  const auto site_id_index = sites_index + 1;
  if (site_id_index >= parts.size()) {
    LOG_ERROR << "url ended without a site ID: " << redactedUrl(url);
    return -1;
  }

  const auto site_id_str = parts[site_id_index];

  bool ok = false;
  const auto site_id = site_id_str.toInt(&ok);

  if (!ok) {
    LOG_ERROR << "site ID is not an integer: " << redactedUrl(url);
    return -1;
  }

  return site_id;
}

QNetworkRequest createAuthRequest(const QAuthenticator& auth) {
  auto url = createUrl(path_auth);
  QUrlQuery query;
  query.addQueryItem("email", auth.user());
  query.addQueryItem("password", auth.password());
  url.setQuery(query);

  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");

  return request;
}

QNetworkRequest createSitesRequest(const QString& api_token) {
  auto url = createUrl(path_sites);
  QUrlQuery query;
  query.addQueryItem("token", api_token);
  url.setQuery(query);
  return QNetworkRequest(url);
}

QNetworkRequest createDownloadsRequest(const QString& api_token,
                                       const int site_id) {
  const auto path =
      QString("%1/%2%3").arg(path_sites).arg(site_id).arg(path_downloads);
  auto url = createUrl(path);
  QUrlQuery query;
  query.addQueryItem("token", api_token);
  url.setQuery(query);
  QNetworkRequest request(url);
  return request;
}

QJsonObject jsonFromReply(QNetworkReply* reply) {
  QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
  return jsonDoc.object();
}

std::vector<GondarSite> sitesFromReply(QNetworkReply* reply) {
  const QJsonArray rawSites = jsonFromReply(reply)["sites"].toArray();
  std::vector<GondarSite> sites;

  for (const QJsonValue& cur : rawSites) {
    const QJsonObject site = cur.toObject();
    const auto site_id = site["site_id"].toInt();
    const auto site_name = site["name"].toString();
    sites.emplace_back(GondarSite(site_id, site_name));
  }

  return sites;
}

}  // namespace

namespace gondar {

Meepo::Meepo() {
  // All replies are handled by dispatchReply
  connect(getNetworkManager(), &QNetworkAccessManager::finished,
          this, &Meepo::dispatchReply);
}

void Meepo::start(const QAuthenticator& auth) {
  LOG_INFO << "starting meepo flow";

  api_token_.clear();
  sites_.clear();
  error_.clear();
  sites_remaining_ = 0;

  requestAuth(auth);
}

QString Meepo::error() const {
  return error_;
}

Meepo::Sites Meepo::sites() const {
  return sites_;
}

void Meepo::requestAuth(const QAuthenticator& auth) {
  const auto request = createAuthRequest(auth);
  LOG_INFO << "POST " << redactedUrl(request.url());
  getNetworkManager()->post(request, QByteArray());
}

void Meepo::handleAuthReply(QNetworkReply* reply) {
  api_token_ = jsonFromReply(reply)["api_token"].toString();

  if (api_token_.isEmpty()) {
    fail("invalid API token");
    return;
  }

  LOG_INFO << "token received";
  requestSites();
}

void Meepo::requestSites() {
  const auto request = createSitesRequest(api_token_);
  LOG_INFO << "GET " << redactedUrl(request.url());
  getNetworkManager()->get(request);
}

void Meepo::handleSitesReply(QNetworkReply* reply) {
  sites_ = sitesFromReply(reply);
  LOG_INFO << "received " << sites_.size() << " site(s)";

  sites_remaining_ = sites_.size();

  for (const auto& site : sites_) {
    requestDownloads(site);
  }
}

void Meepo::requestDownloads(const GondarSite& site) {
  const auto request = createDownloadsRequest(api_token_, site.getSiteId());
  LOG_INFO << "GET " << redactedUrl(request.url());
  getNetworkManager()->get(request);
}

void Meepo::handleDownloadsReply(QNetworkReply* reply) {
  const auto site_id = siteIdFromUrl(reply->url());
  if (site_id == -1) {
    fail("missing site ID");
    return;
  }

  GondarSite* site = siteFromSiteId(site_id);
  if (!site) {
    fail("site not found");
    return;
  }

  const auto jsonObj = jsonFromReply(reply);
  QJsonObject downloadsObj = jsonObj["links"].toObject();
  // for starters, let's just use use the cloudready product
  QJsonValue downloadsValue = downloadsObj["CloudReady"];
  QJsonArray downloadsArray = downloadsValue.toArray();
  // for starters, let's just use "32-bit" and "64-bit" builds
  for (int i = 0; i < downloadsArray.size(); i++) {
    QJsonObject download = downloadsArray.at(i).toObject();
    if (download["title"] == "64-Bit") {
      QUrl url64(download["url"].toString());
      site->set64Url(url64);
    } else if (download["title"] == "32-Bit") {
      QUrl url32(download["url"].toString());
      site->set32Url(url32);
    }
  }

  sites_remaining_--;

  // see if we're done
  if (sites_remaining_ == 0) {
    LOG_INFO << "received information for all outstanding site requests";
    // we don't want users to be able to pass through the screen by pressing
    // next while processing.  this will make validatePage pass and immediately
    // move the user on to the next screen
    emit finished();
  }
}

void Meepo::dispatchReply(QNetworkReply* reply) {
  const auto error = reply->error();
  const auto url = reply->url();

  if (error != QNetworkReply::NoError) {
    // TODO(nicholasbishop): make this more readable
    LOG_ERROR << "network error: " << redactedUrl(url) << ", error " << error;
    // TODO(nicholasbishop): move the error handling into each of the
    // three handlers below so that errors can be more specific
    fail("network error");
  } else if (url.path().endsWith("/auth")) {
    handleAuthReply(reply);
  } else if (url.path().endsWith("/sites")) {
    handleSitesReply(reply);
  } else if (url.path().endsWith("/downloads")) {
    handleDownloadsReply(reply);
  }

  reply->deleteLater();
}

void Meepo::fail(const QString& error) {
  LOG_ERROR << "error";
  error_ = error;
  emit finished();
}

GondarSite* Meepo::siteFromSiteId(const int site_id) {
  for (auto& site : sites_) {
    if (site.getSiteId() == site_id) {
      return &site;
    }
  }
  return nullptr;
}

}  // namespace gondar
