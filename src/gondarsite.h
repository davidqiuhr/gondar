// Copyright 2017 Neverware
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SRC_GONDARSITE_H_
#define SRC_GONDARSITE_H_

#include <QString>
#include <QUrl>

#include "gondarimage.h"

class GondarSite {
 public:
  GondarSite(int siteIdIn, QString siteNameIn);
  int getSiteId() const;
  const QString& getSiteName() const;
  QList<GondarImage> getImages() const;
  void addImage(const GondarImage& image);

 private:
  int siteId;
  QString siteName;
  QList<GondarImage> imageList;
  QUrl url32;
  QUrl url64;
};

#endif  // SRC_GONDARSITE_H_
