#include "chromeover_login_page.h"

#include <QDebug>

#include "gondarsite.h"
#include "gondarwizard.h"

ChromeoverLoginPage::ChromeoverLoginPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Login");
  setSubTitle("Enter credentials for my.neverware.com.");
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));
  // configure username fields
  usernameLineEditLabel.setBuddy(&usernameLineEdit);
  usernameLineEditLabel.setText("Username:");
  layout.addWidget(&usernameLineEditLabel, 0, 0);
  layout.addWidget(&usernameLineEdit, 0, 1);
  // configure password fields
  passwordLineEdit.setEchoMode(QLineEdit::Password);
  passwordLineEditLabel.setBuddy(&passwordLineEdit);
  passwordLineEditLabel.setText("Password:");
  meanWordsLabel.setText("That's not right!  Try again.");
  // give it the smithforrestr treatment
  meanWordsLabel.setStyleSheet("QLabel { color : red; }");
  layout.addWidget(&passwordLineEditLabel, 1, 0);
  layout.addWidget(&passwordLineEdit, 1, 1);
  layout.addWidget(&meanWordsLabel, 2, 0);
  meanWordsLabel.setVisible(false);
  setLayout(&layout);
  // not sure about this design choice yet
  outstandingSites = 0;
  finished = false;
  // don't allow multiple launches of this logic
  started = false;
}

int ChromeoverLoginPage::nextId() const {
  //TODO: if we have more than two sites, go to Page_siteSelect
  // otherwise, go to Page_imageSelect
  //TODO: think about the bypass case; where will the imaages get updated?
  // is it too weird to just do it in this page's logic like i had it initially?
  if (siteList.size() == 0) {
    return GondarWizard::Page_error;
  } else if (siteList.size() > 1) {
    return GondarWizard::Page_siteSelect;
  } else {
    wizard()->imageSelectPage.set64Url(siteList[0]->url64);
    wizard()->imageSelectPage.set32Url(siteList[0]->url32);
    // update the urls here
    return GondarWizard::Page_imageSelect;
  }
}

bool ChromeoverLoginPage::validatePage() {
  // TODO: let's grab the field contents here
  // these work
  QString username = usernameLineEdit.text();
  QString password = passwordLineEdit.text();
  if (finished) {
    return true;
  }
  if (!started) {
    // step 1: log in and get api token
    startGetToken(username, password);
  }
  started = true;
  return false;
}

void ChromeoverLoginPage::startGetToken(QString username, QString password) {
  QUrl tokenFetchUrl("https://api.grv.neverware.com/poof/auth");
  QUrlQuery query;
  query.addQueryItem("email", username);
  query.addQueryItem("password", password);
  tokenFetchUrl.setQuery(query);
  connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
          SLOT(tokenRequestFinished(QNetworkReply*)));
  QNetworkRequest request(tokenFetchUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  QSslConfiguration config = QSslConfiguration::defaultConfiguration();
  request.setSslConfiguration(config);
  QByteArray empty;
  networkManager.post(request, empty);
}

void ChromeoverLoginPage::tokenRequestFinished(QNetworkReply* reply) {
  disconnect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
             SLOT(tokenRequestFinished(QNetworkReply*)));
  // TODO: investigate to what extent this covers all the error response codes
  if (reply->error()) {
    qDebug() << "Invalid login credentials supplied";
    started = false;
    meanWordsLabel.setVisible(true);
    return;
  }

  apiToken = getToken(reply);
  reply->deleteLater();
  // great so we have the token maybe.  now we use it to list our download sites
  QUrl listSitesUrl("https://api.grv.neverware.com/poof/sites");
  // this time we will want to add params like i had earlier
  QUrlQuery query;
  query.addQueryItem("token", apiToken);
  listSitesUrl.setQuery(query);
  QNetworkRequest request(listSitesUrl);
  networkManager.get(request);
  connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
          SLOT(sitesRequestFinished(QNetworkReply*)));
}

QString ChromeoverLoginPage::getToken(QNetworkReply* reply) {
  QString replyStr = (QString)reply->readAll();
  QJsonDocument jsonDoc = QJsonDocument::fromJson(replyStr.toUtf8());
  QJsonObject jsonObj = jsonDoc.object();
  return jsonObj["api_token"].toString();
}

void ChromeoverLoginPage::sitesRequestFinished(QNetworkReply* reply) {
  disconnect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
             SLOT(sitesRequestFinished(QNetworkReply*)));

  QString replyStr = (QString)reply->readAll();
  QJsonDocument jsonDoc = QJsonDocument::fromJson(replyStr.toUtf8());
  QJsonObject jsonObj = jsonDoc.object();
  QJsonValue sitesObj = jsonObj["sites"];
  QJsonArray sitesArray = sitesObj.toArray();

  // first connect our listener guy
  connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
          SLOT(imageUrlRequestFinished(QNetworkReply*)));
  // for site in sites, go get the image endpoints for those sites

  for (int i = 0; i < sitesArray.size(); i++) {
    QJsonObject cur = sitesArray.at(i).toObject();
    int siteId = cur["site_id"].toInt();
    QString siteIdStr = QString::number(siteId);
    QString site_name = cur["name"].toString();
    GondarSite * cur_site = new GondarSite(siteId, site_name);
    siteList << cur_site;
    // we want to ask for the downloads for this site
    QUrl downloadsUrl("https://api.grv.neverware.com/poof/sites/" + siteIdStr +
                      "/downloads");
    // this time we will want to add params like i had earlier
    QUrlQuery query;
    query.addQueryItem("token", apiToken);
    downloadsUrl.setQuery(query);
    QNetworkRequest request(downloadsUrl);
    networkManager.get(request);
    outstandingSites++;
  }
}

void ChromeoverLoginPage::imageUrlRequestFinished(QNetworkReply* reply) {
  // this time it's a bit interesting, as we don't disconnect our listener
  // we only disconnect once all the sites have been accounted for

  // we find out which site this response pertains to
  // then we remove it from our list of unprocessed sites
  // then, if the list of unprocessed sites is empty, we run disconnect
  // then we populate whatever struct info we need for the processed sites

  // the site number will be third symbol deep splitting on / of path
  QString siteName = reply->request().url().path().split("/").at(3);
  //TODO: find the site entry we made in sitesRequestFinished 
  int siteNum = siteName.toInt();
  GondarSite * thisSite = NULL;
  for (int i = 0; i < siteList.size(); i++) {
    if (siteList[i]->siteId == siteNum) {
      thisSite = siteList[i];
    }
  }
  if (thisSite == NULL) {
    // not really sure when this would happen
    qDebug() << "ERROR: site not found!";
  }

  QString replyStr = (QString)reply->readAll();
  QJsonDocument jsonDoc = QJsonDocument::fromJson(replyStr.toUtf8());
  QJsonObject jsonObj = jsonDoc.object();
  QJsonObject downloadsObj = jsonObj["links"].toObject();
  for (int i = 0; i < downloadsObj.size(); i++) {
    QJsonValue kewlvalue = downloadsObj.keys().at(i);
  }
  // for starters, let's just use cloudready
  QJsonValue downloadsValue = downloadsObj["CloudReady"];
  QJsonArray downloadsArray = downloadsValue.toArray();
  // for starters, let's just use "32-bit" and "64-bit"

  for (int i = 0; i < downloadsArray.size(); i++) {
    QJsonObject download = downloadsArray.at(i).toObject();
    if (download["title"] == "64-Bit") {
      QUrl url64(download["url"].toString());
      //FIXME: if there are multiple sites, whatever site responds last
      // will get to use its values.  that is kind of a weird behavior
      // even for an initial pass.
      //wizard()->imageSelectPage.set64Url(url64);
      thisSite->set64Url(url64);
    } else if (download["title"] == "32-Bit") {
      QUrl url32(download["url"].toString());
      //wizard()->imageSelectPage.set32Url(url32);
      thisSite->set32Url(url32);
    }
  }

  // see if we're done
  outstandingSites--;
  if (outstandingSites == 0) {
    qDebug() << "received information for all outstanding site requests";
    disconnect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
               SLOT(imageUrlRequestFinished(QNetworkReply*)));
    finished = true;
    wizard()->next();
  }
}
