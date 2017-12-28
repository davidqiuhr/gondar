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

#include "metric.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>
#include <QUuid>

#include "config.h"
#include "gondarsite.h"
#include "log.h"
#include "util.h"

namespace gondar {

namespace {

QNetworkAccessManager* getNetworkManager() {
  static QNetworkAccessManager manager;
  return &manager;
}

std::string getMetricString(Metric metric) {
  switch (metric) {
    case Metric::BeeroverUse:
      return "beerover-use";
    case Metric::ChromeoverUse:
      return "chromeover-use";
    case Metric::DownloadAttempt:
      return "download-attempt";
    case Metric::DownloadSuccess:
      return "download-success";
    case Metric::DownloadFailure:
      return "download-failure";
    case Metric::Error:
      return "error";
    case Metric::UsbAttempt:
      return "usb-attempt";
    case Metric::UsbSuccess:
      return "usb-success";
    case Metric::Use:
      return "use";
    case Metric::SuccessDuration:
      return "success-duration";
    // not sure we want to crash the program on a bad metric lookup
    default:
      return "unknown";
  }
}

QByteArray getMetricsApiKey() {
#ifdef METRICS_API_KEY
  return QByteArray(METRICS_API_KEY);
#else
  return QByteArray();
#endif
}
}

static QString getUuid() {
  static QString id;
  // if we've already initialized the UUID this program run, use the old value
  if (!id.isEmpty()) {
    return id;
  }
  // otherwise, go look up or make the value
  const QDir dir =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  QString filepath = dir.filePath("cloudready_installer_uuid");
  QFile uuidFile(filepath);
  // attempt to get an existing uuid
  try {
    id = readUtf8File(filepath).trimmed();
    if (id.size() != 38) {
      throw std::runtime_error("invalid UUID size");
    }
  } catch (const std::exception& err) {
    LOG_INFO << "Creating new UUID";
    // then we make our uuid
    id = QUuid::createUuid().toString();
    uuidFile.open(QIODevice::WriteOnly);
    QTextStream outstream(&uuidFile);
    outstream << id;
  }
  return id;
}

static int SetGetSiteId(int site_id_in) {
  static int site_id = 0;
  if (site_id_in != 0) {
    site_id = site_id_in;
  }
  return site_id;
}

void SetSiteId(const std::vector<GondarSite>& sites) {
  int min_site_id = 1000000; // if we have more than one million sites and it
                             // causes a bug, we will at least be very wealthy
  for (GondarSite site : sites) {
    if (site.getSiteId() < min_site_id) {
      min_site_id = site.getSiteId();
    }
  }
  if (min_site_id != 1000000) {
    SetGetSiteId(min_site_id);
  }
}

void SendMetric(Metric metric, const std::string& value) {
  const auto api_key = getMetricsApiKey();
  if (api_key.isEmpty()) {
    // all production builds should sent metrics
    LOG_WARNING << "not sending metrics!";
    return;
  }
  std::string metricStr = getMetricString(metric);
  QNetworkAccessManager* manager = getNetworkManager();
  QUrl url("https://gondar-metrics.neverware.com/prod");
  QJsonObject json;
  QString id = getUuid();
  json["identifier"] = id;
  json.insert("metric", QString::fromStdString(metricStr));
  if (!value.empty()) {
    // then we append the value to the metric
    json.insert("value", QString::fromStdString(value));
  }
  const auto version = gondar::getGondarVersion();
  if (!version.isEmpty()) {
    json.insert("version", version);
  }
  QString product;
  if (gondar::isChromeover()) {
    product = "chromeover";
  } else {
    product = "beerover";
  }
  json.insert("product", product);
  const auto siteId = SetGetSiteId(0);
  // ignore uninitialized and beerover cases
  if (! (siteId == 0 || !isChromeover())) {
    json.insert("site", siteId);
  }
  QNetworkRequest request(url);
  request.setRawHeader(QByteArray("x-api-key"), api_key);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  QJsonDocument doc(json);
  QString strJson(doc.toJson(QJsonDocument::Compact));
  manager->post(request, QByteArray(strJson.toUtf8()));
}
}
