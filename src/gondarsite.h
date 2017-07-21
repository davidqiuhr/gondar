#ifndef GONDARSITE_H
#define GONDARSITE_H

#include <QString>
#include <QUrl>

class GondarSite {
 public:
  GondarSite(int siteIdIn, QString siteNameIn);
  int getSiteId() const;
  const QString& getSiteName() const;
  void set32Url(QUrl url);
  void set64Url(QUrl url);
  QUrl get32Url() const;
  QUrl get64Url() const;

 private:
  int siteId;
  QString siteName;
  QUrl url32;
  QUrl url64;
};

#endif /* GONDARSITE_H */
