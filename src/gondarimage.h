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

#ifndef GONDARIMAGE_H
#define GONDARIMAGE_H

class GondarImage {
  public:
    GondarImage() {
        this->product = QString("");
        this->imageName = QString("");
        this->url = QUrl();
    }
    GondarImage(QString productIn, QString imageNameIn, QUrl urlIn) {
        this->product = productIn;
        this->imageName = imageNameIn;
        this->url = urlIn;
    }
    QString getCompositeName() {
        return product + " " + imageName;
    }
    bool is32Bit() {
        return imageName.contains("32-bit", Qt::CaseInsensitive);
    }
    bool isDeployable() {
        return imageName.contains("deployable", Qt::CaseInsensitive);
    }
    QString product;
    QString imageName;
    QUrl url;
};

#endif
