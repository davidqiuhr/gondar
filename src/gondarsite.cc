
#include "gondarsite.h"
#include <QDebug>

GondarSite::GondarSite(int siteIdIn, QString siteNameIn) {
  siteId = siteIdIn;
  siteName = siteNameIn;
}

int GondarSite::getSiteId() const {
  return siteId;
}

const QString& GondarSite::getSiteName() const {
  return siteName;
}

void GondarSite::set32Url(QUrl url) {
  url32 = url;
}

void GondarSite::set64Url(QUrl url) {
  url64 = url;
}

QUrl GondarSite::get32Url() const {
  return url32;
}

QUrl GondarSite::get64Url() const {
  return url64;
}
