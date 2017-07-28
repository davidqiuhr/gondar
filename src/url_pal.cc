
#include "url_pal.h"

#include <QNetworkReply>
#include <QString>
#include <QUrl>
#include "log.h"

UrlPal::UrlPal() {
  url32Ready = false;
  url64Ready = false;
}

void UrlPal::fetch() {
  QString baseUrl(
      "https://s3.amazonaws.com/neverware-cloudready-free-releases/");
    // for the free version, we have to find out what the url is
  connect(&networkManager, &QNetworkAccessManager::finished, this, &UrlPal::handleReply);
  networkManager.get(QNetworkRequest(QUrl(baseUrl + "latest-stable-64bit")));
  networkManager.get(QNetworkRequest(QUrl(baseUrl + "latest-stable-32bit")));
}

void UrlPal::handleReply(QNetworkReply* reply) {
  const auto error = reply->error();
  if (error != QNetworkReply::NoError) {
    LOG_ERROR << "Error retrieving CloudReady Home Edition URL";
  }
  // we find out which request this is a response for
  // TODO: can readAll() be trusted to read our path strings properly?
  QUrl url = QUrl(QString("https://ddnynf025unax.cloudfront.net/") + reply->readAll());
  if (reply->url().toString().contains("32bit")) {
    thirtyTwoUrl = url;
    url32Ready = true;
  } else {
    sixtyFourUrl = url;
    url64Ready = true;
  }
  qDebug() << reply->url();
  qDebug() << reply->readAll();
  reply->deleteLater();
}

void UrlPal::set32Url(QUrl url_in) {
  thirtyTwoUrl = url_in;
}

void UrlPal::set64Url(QUrl url_in) {
  sixtyFourUrl = url_in;
}

bool UrlPal::isReady() {
  if (thirtyTwoUrl.isEmpty() || sixtyFourUrl.isEmpty()) {
    return false;
  } else {
    return true;
  }
}

QUrl UrlPal::get32Url() {
  return thirtyTwoUrl;
}

QUrl UrlPal::get64Url() {
  return sixtyFourUrl;
}
