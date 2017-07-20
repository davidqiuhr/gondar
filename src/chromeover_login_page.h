#ifndef CHROMEOVER_LOGIN_PAGE_H
#define CHROMEOVER_LOGIN_PAGE_H

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QNetworkReply>
#include "gondarsite.h"
#include "wizard_page.h"

class ChromeoverLoginPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  ChromeoverLoginPage(QWidget* parent = 0);
  int nextId() const override;
  QList<GondarSite*> siteList;

 protected:
  void initializePage() override;
  bool validatePage() override;

 private:
  void startGetToken(QString username, QString password);
  QString getToken(QNetworkReply* reply);
  QNetworkAccessManager networkManager;
  QGridLayout layout;
  QLineEdit usernameLineEdit;
  QLabel usernameLineEditLabel;
  QLineEdit passwordLineEdit;
  QLabel passwordLineEditLabel;
  QLabel meanWordsLabel;
  int outstandingSites;
  QString apiToken;
  bool finished;
  bool started;
 public slots:
  void tokenRequestFinished(QNetworkReply* reply);
  void sitesRequestFinished(QNetworkReply* reply);
  void imageUrlRequestFinished(QNetworkReply* reply);
};

#endif
