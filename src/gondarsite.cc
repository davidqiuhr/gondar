
#include <QDebug>
#include "gondarsite.h"

GondarSite::GondarSite(int siteIdIn, QString siteNameIn) {
  siteId = siteIdIn;
  siteName = siteNameIn;
  setText(siteName);
}

void GondarSite::set32Url(QUrl url) {
  url32 = url;
}

void GondarSite::set64Url(QUrl url) {
  url64 = url;
}
