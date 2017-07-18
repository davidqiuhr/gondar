#ifndef GONDARSITE_H
#define GONDARSITE_H

#include <QString>
#include <QUrl>

class GondarSite {
 public:
  GondarSite(QString siteName, QUrl url32, QUrl url64);
  QString siteName;
  QUrl url32;
  QUrl url64;
};

#endif /* GONDARSITE_H */
