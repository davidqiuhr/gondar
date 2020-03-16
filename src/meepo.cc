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

#include "meepo.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QUrlQuery>

#include "config.h"
#include "gondarsite.h"
#include "log.h"
#include "util.h"

namespace {

const char path_auth[] = "/auth";
const char path_google_auth[] = "/google-auth";
const char path_sites[] = "/sites";
const char path_downloads[] = "/downloads";

QUrl createUrl(const QString& path) {
  return QUrl("https://api." + gondar::getDomain() + "/poof" + path);
}

int siteIdFromUrl(const QUrl& url) {
  const auto path = url.path();
  const auto parts = path.split('/');
  const auto sites_index = parts.lastIndexOf("sites");

  if (sites_index == -1) {
    LOG_ERROR << "failed to find 'sites' in " << url.toString();
    return -1;
  }

  const auto site_id_index = sites_index + 1;
  if (site_id_index >= parts.size()) {
    LOG_ERROR << "url ended without a site ID: " << url.toString();
    return -1;
  }

  const auto site_id_str = parts[site_id_index];

  bool ok = false;
  const auto site_id = site_id_str.toInt(&ok);

  if (!ok) {
    LOG_ERROR << "site ID is not an integer: " << url.toString();
    return -1;
  }

  return site_id;
}

QNetworkRequest createAuthRequest() {
  auto url = createUrl(path_auth);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return request;
}

QNetworkRequest createGoogleAuthRequest() {
  auto url = createUrl(path_google_auth);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return request;
}

QNetworkRequest createSitesRequest(const QString& api_token, int page) {
  auto url = createUrl(path_sites);
  QUrlQuery query;
  query.addQueryItem("token", api_token);
  // QString wants a C string.  we start with an int.
  // so we go int->to_string->c_str()->QString
  // makes sense to me
  query.addQueryItem("page", QString::number(page));
  url.setQuery(query);
  LOG_INFO << "~~ken: createsitesreq = " << url.toString();
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

std::vector<GondarSite> sitesFromReply(const QJsonArray& rawSites) {
  std::vector<GondarSite> sites;

  for (const QJsonValue& cur : rawSites) {
    const QJsonObject site = cur.toObject();
    const auto site_id = site["site_id"].toInt();
    const auto site_name = site["name"].toString();
    sites.emplace_back(GondarSite(site_id, site_name));
  }

  return sites;
}

// TODO(ken): test for this
int getNextPage(const QJsonObject& outer_json) {
  const QJsonObject json = outer_json["pagination"].toObject();
  LOG_INFO << "~~pagination cur = " << json.value("current").toInt();
  LOG_INFO << "~~pagination total = " << json.value("total").toInt();
  auto cur = json.value("current").toInt();
  auto total = json.value("total").toInt();
  if (cur < total) {
    return cur + 1;
  } else {
    return 0;
  }
}

}  // namespace

namespace gondar {

Meepo::Meepo() {
  // All replies are handled by dispatchReply
  connect(&network_manager_, &QNetworkAccessManager::finished, this,
          &Meepo::dispatchReply);
}

void Meepo::clear() {
  api_token_.clear();
  sites_.clear();
  error_.clear();
  sites_remaining_ = 0;
}

void Meepo::start(const QAuthenticator& auth) {
  google_mode_ = false;
  LOG_INFO << "starting meepo flow";
  clear();
  requestAuth(auth);
}

void Meepo::startGoogle(QString id_token) {
  google_mode_ = true;
  LOG_INFO << "starting meepo flow with google";
  clear();
  requestGoogleAuth(id_token);
}

QString Meepo::error() const {
  return error_;
}

Meepo::Sites Meepo::sites() const {
  return sites_;
}

void Meepo::requestAuth(const QAuthenticator& auth) {
  QJsonObject json;
  json["email"] = auth.user();
  json["password"] = auth.password();
  QJsonDocument doc(json);
  auto request = createAuthRequest();
  LOG_INFO << "POST " << request.url().toString();
  network_manager_.post(request, doc.toJson(QJsonDocument::Compact));
}

void Meepo::requestGoogleAuth(QString id_token) {
  QJsonObject json;
  json["id_token"] = id_token;
  LOG_INFO << "id_token =" << id_token;
  QJsonDocument doc(json);
  auto request = createGoogleAuthRequest();
  LOG_INFO << "POST " << request.url().toString();
  network_manager_.post(request, doc.toJson(QJsonDocument::Compact));
}

void Meepo::handleAuthReply(QNetworkReply* reply) {
  api_token_ = jsonFromReply(reply)["api_token"].toString();

  if (api_token_.isEmpty()) {
    fail("invalid API token");
    return;
  }

  LOG_INFO << "token received";
  // FIXME(ken): ok now we're back to the mystery of why this page
  // arg is not being used
  requestSites(1);
}

void Meepo::requestSites(int page) {
  const auto request = createSitesRequest(api_token_, page);
  LOG_INFO << "GET " << request.url().toString();
  network_manager_.get(request);
}

// FIXME(ken): what i don't understand is why am i getting all the downloads
// for the sites i do not have an interest in?
// it feels like i should choose a site, hit next, then validate page does
// a transaction to find the images for that site before calling next()

// the weird thing about this function in its new reality is that it will be
// called on each page; it's basically recursive like a nightmare
// we should probably set a max pages to like 100
void Meepo::handleSitesReply(QNetworkReply* reply) {
  const QJsonObject json = gondar::jsonFromReply(reply);
  const QJsonArray rawSites = json["sites"].toArray();
  // TODO(ken): lower priority but maybe rename?
  sites_ = sitesFromReply(rawSites);
  
  LOG_INFO << "received " << sites_.size() << " site(s)";

  // sites starts at zero, and every time we go get another page,
  // there are more sites remaining to get downloads from
  sites_remaining_ += sites_.size();
  LOG_INFO << "~~KEN: sites_remaining increased, now " << sites_remaining_;

  // special handling for the zero sites case
  if (sites_remaining_ == 0) {
    fail(no_sites_error);
    return;
  }

  // FIXME(ken): i never handle the first page (5 results)
  // only the second page (two results)
  for (const auto& site : sites_) {
    requestDownloads(site);
  }

  // see if there's another batch
  int next_page = getNextPage(json);
  LOG_INFO << "~~~KEN: next page = " << next_page;
  if (next_page > 0) {
    requestSites(next_page);
  }
}

void Meepo::requestDownloads(const GondarSite& site) {
  const auto request = createDownloadsRequest(api_token_, site.getSiteId());
  LOG_INFO << "GET " << request.url().toString();
  network_manager_.get(request);
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
  QJsonObject productsObj = jsonObj["links"].toObject();

  for (const auto& product : productsObj.keys()) {
    for (const auto& image : productsObj[product].toArray()) {
      const auto imageObj = image.toObject();
      const auto imageName(imageObj["title"].toString());
      const QUrl url(imageObj["url"].toString());
      LOG_INFO << "Product: " << product << ", Image name:" << imageName;
      GondarImage gondarImage(product, imageName, url);
      site->addImage(gondarImage);
    }
  }
  sites_remaining_--;
  // FIXME(ken): this is not being called for that first set of 5 requests
  LOG_INFO << "~~KEN: sites_remaining lowered, now " << sites_remaining_;

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
    LOG_ERROR << "network error: " << url.toString() << ", error " << error;
    // TODO(nicholasbishop): move the error handling into each of the
    // three handlers below so that errors can be more specific
    fail("network error");
    // FIXME: use the constants at top of file instead
  } else if (url.path().endsWith("/auth")) {
    handleAuthReply(reply);
  } else if (url.path().endsWith("/google-auth")) {
    handleAuthReply(reply);
  // FIXME(ken): this might be a pickle...will the url path still end with
  // /sites or will it have ?page=1 now?
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
  emit failed(google_mode_);
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
