
#ifndef NEWEST_IMAGE_URL_H
#define NEWEST_IMAGE_URL_H

#include <QUrl>
#include <QNetworkReply>

class NewestImageUrl : public QObject {
  Q_OBJECT

 public:
  NewestImageUrl();
  void fetch();
  bool isReady();
  void set32Url(QUrl url_in);
  void set64Url(QUrl url_in);
  QUrl get32Url();
  QUrl get64Url();
 signals:
  void errorOccurred();
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
