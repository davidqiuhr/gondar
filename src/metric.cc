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
#include <QUrlQuery>
#include <QUuid>

#include "config.h"
#include "log.h"
#include "meepo.h"
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
    case Metric::FormatAttempt:
      return "format-attempt";
    case Metric::FormatSuccess:
      return "format-success";
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
}  // namespace

QString GetUuid() {
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

// the metrics layer stores the site id to provide in later metrics
// once it is available
// FIXME(ken): really?  static state like this?
namespace {
static int site_id = 0;
}

void SetSiteId(int site_id_in) {
  site_id = site_id_in;
}

int GetSiteId() {
  return site_id;
}

static bool shouldSendMetrics() {
  const auto api_key = getMetricsApiKey();
  if (api_key.isEmpty()) {
    // all production builds should sent metrics
    LOG_WARNING << "not sending metrics!";
    return false;
  }
  return true;
}

// send regular gondar metrics
void SendMetricGondar(Metric metric, const std::string& value) {
  if (!shouldSendMetrics()) {
    return;
  }
  LOG_WARNING << "sending a Klassic Metric";
  const auto api_key = getMetricsApiKey();
  std::string metricStr = getMetricString(metric);
  QNetworkAccessManager* manager = getNetworkManager();
  QUrl url("https://gondar-metrics.neverware.com/prod");
  QJsonObject json;
  QString id = GetUuid();
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
  const auto siteId = GetSiteId();
  // only show site when on chromeover and site id has been initialized
  if (isChromeover() && siteId != 0) {
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

static void SendMetricMeepo(Metric metric,
                            const std::string& value,
                            GondarWizard* wizard) {
  // don't send metrics to meepo if we're not configured to send metrics
  // technically we could, but for simplicity all metrics will be on/off
  // together.
  if (!shouldSendMetrics()) {
    return;
  }
  LOG_WARNING << "sending a Meepo Metric";
  wizard->meepo_.sendMetric();
}

void SendMetric(GondarWizard* wizard, Metric metric, const std::string& value) {
  LOG_WARNING << "HIT OUTER METRIC GUY";
  SendMetricGondar(metric, value);
  // if we have a token, also send the metric to meepo
  // FIXME(ken): seems like we could just send in the whole meepo instance,
  // then most of the handling could be in meepo
  if (wizard && wizard->meepo_.hasToken()) {
    SendMetricMeepo(metric, value, wizard);
  }
}
}  // namespace gondar
