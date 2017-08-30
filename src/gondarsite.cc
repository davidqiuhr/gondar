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

QList<GondarImage> GondarSite::getImages() const {
  return imageList;
}
void GondarSite::addImage(QString product, QString imageName, QUrl url) {
  GondarImage image(product, imageName, url);
  imageList.append(image);
}
