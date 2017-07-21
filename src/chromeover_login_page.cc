#include "chromeover_login_page.h"

#include "gondarsite.h"
#include "gondarwizard.h"
#include "log.h"

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
  outstandingSites = 0;
  // don't allow progressing to next page yet
  finished = false;
  // don't allow another launch of server interaction while another is running
  started = false;
}

int ChromeoverLoginPage::nextId() const {
  if (siteList.size() == 0) {
    return GondarWizard::Page_error;
  } else if (siteList.size() > 1) {
    // if there are multiple sites, we'll set the urls on the site select page
    return GondarWizard::Page_siteSelect;
  } else {
    // otherwise, we can skip that step and update the urls here
    wizard()->imageSelectPage.set64Url(siteList[0]->get64Url());
    wizard()->imageSelectPage.set32Url(siteList[0]->get32Url());
    return GondarWizard::Page_imageSelect;
  }
}

bool ChromeoverLoginPage::validatePage() {
  QString username = usernameLineEdit.text();
  QString password = passwordLineEdit.text();
  if (finished) {
    return true;
  }
  if (!started) {
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
    LOG_INFO << "Invalid login credentials supplied";
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
  QJsonArray sitesArray = jsonDoc.object()["sites"].toArray();

  // first connect our listener guy
  connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
          SLOT(imageUrlRequestFinished(QNetworkReply*)));
  // for site in sites, go get the image endpoints for those sites
  for (int i = 0; i < sitesArray.size(); i++) {
    QJsonObject cur = sitesArray.at(i).toObject();
    int siteId = cur["site_id"].toInt();
    QString siteIdStr = QString::number(siteId);
    QString siteName = cur["name"].toString();
    // FIXME: leak
    GondarSite* curSite = new GondarSite(siteId, siteName);
    siteList << curSite;
    // we want to ask for the downloads for this site
    QUrl downloadsUrl("https://api.grv.neverware.com/poof/sites/" + siteIdStr +
                      "/downloads");
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
  int siteNum = siteName.toInt();
  GondarSite* thisSite = NULL;
  for (int i = 0; i < siteList.size(); i++) {
    if (siteList[i]->getSiteId() == siteNum) {
      thisSite = siteList[i];
    }
  }
  if (thisSite == NULL) {
    LOG_ERROR << "ERROR: site not found!";
  }

  QString replyStr = (QString)reply->readAll();
  QJsonDocument jsonDoc = QJsonDocument::fromJson(replyStr.toUtf8());
  QJsonObject jsonObj = jsonDoc.object();
  QJsonObject downloadsObj = jsonObj["links"].toObject();
  // for starters, let's just use use the cloudready product
  QJsonValue downloadsValue = downloadsObj["CloudReady"];
  QJsonArray downloadsArray = downloadsValue.toArray();
  // for starters, let's just use "32-bit" and "64-bit" builds
  for (int i = 0; i < downloadsArray.size(); i++) {
    QJsonObject download = downloadsArray.at(i).toObject();
    if (download["title"] == "64-Bit") {
      QUrl url64(download["url"].toString());
      thisSite->set64Url(url64);
    } else if (download["title"] == "32-Bit") {
      QUrl url32(download["url"].toString());
      thisSite->set32Url(url32);
    }
  }

  // see if we're done
  outstandingSites--;
  if (outstandingSites == 0) {
    LOG_INFO << "received information for all outstanding site requests";
    disconnect(&networkManager, SIGNAL(finished(QNetworkReply*)), this,
               SLOT(imageUrlRequestFinished(QNetworkReply*)));
    // we don't want users to be able to pass through the screen by pressing
    // next while processing.  this will make validatePage pass and immediately
    // move the user on to the next screen
    finished = true;
    // TODO: let's make this a commit page so we don't have to worry about
    // the case of a user revisiting this segment for now
    wizard()->next();
  }
}
