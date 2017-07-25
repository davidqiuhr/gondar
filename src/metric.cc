
#cmakedefine METRICS_API_KEY "@METRICS_API_KEY@"

#include "metric.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>

namespace gondar {

QNetworkAccessManager* getNetworkManager() {
  static QNetworkAccessManager manager;
  return & manager;
}

std::string getMetricString(Metric metric) {
  switch (metric) {
    case Use:  return "use";
    // not sure we want to crash the program on a bad metric lookup
    default:   return "unknown";
  }
}

void SendMetric(Metric metric) {
    // TODO: add a metrics enabled bool in cmake layer and return here
    // if metrics are disabled
    std::string metricStr = getMetricString(metric);
    QNetworkAccessManager * manager = getNetworkManager();
    QUrl url("https://4mjpbmflkd.execute-api.us-east-1.amazonaws.com/prod");
    QJsonObject json;
    // TODO: use a persistent UUID across a session, and potentially even
    // across multiple runs
    QString id = QUuid::createUuid().toString();
    json["identifier"] = id;
    json.insert("metric", QString::fromStdString(metricStr));
    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("x-api-key"),
                         METRICS_API_KEY);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");
    QJsonDocument doc(json);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    manager->post(request, QByteArray(strJson.toUtf8()));
}

}
