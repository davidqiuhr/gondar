
#include "gondarsite.h"
#include <QDebug>

GondarSite::GondarSite(int siteIdIn, QString siteNameIn) {
  siteId = siteIdIn;
  siteName = siteNameIn;
  setText(siteName);
}

int GondarSite::getSiteId() {
  return siteId;
}

void GondarSite::set32Url(QUrl url) {
  url32 = url;
}

void GondarSite::set64Url(QUrl url) {
  url64 = url;
}

QUrl GondarSite::get32Url() {
  return url32;
}

QUrl GondarSite::get64Url() {
  return url64;
}
