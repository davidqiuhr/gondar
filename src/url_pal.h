
#ifndef BEEROVER_URL_FETCHER_H
#define BEEROVER_URL_FETCHER_H

#include <QUrl>
#include <QNetworkReply>

class UrlPal : public QObject {
  Q_OBJECT

 public:
  UrlPal();
  void fetch();
  bool isReady();
  void set32Url(QUrl url_in);
  void set64Url(QUrl url_in);
  QUrl get32Url();
  QUrl get64Url();
 protected:
  void handleReply(QNetworkReply* reply);
 private:
  QNetworkAccessManager networkManager;
  bool url32Ready;
  bool url64Ready;
  QUrl thirtyTwoUrl;
  QUrl sixtyFourUrl;
};

#endif
