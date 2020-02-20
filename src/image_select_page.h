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

#ifndef SRC_IMAGE_SELECT_PAGE_H_
#define SRC_IMAGE_SELECT_PAGE_H_

#include <QButtonGroup>
#include <QLabel>
#include <QRadioButton>
#include <QUrl>
#include <QVBoxLayout>

#include "newest_image_url.h"

#include "wizard_page.h"

#include "gondarimage.h"

class ImageSelectPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  explicit ImageSelectPage(QWidget* parent = 0);
  QUrl getUrl() const;
  int nextId() const override;
  void addImage(GondarImage image);
  void addImages(QList<GondarImage> images);
  QUrl getUrl();
  void handleNewestImageUrlError();

 protected:
  void initializePage() override;
  bool validatePage() override;

 private:
  QButtonGroup bitnessButtons;
  QRadioButton sixtyFour;
  QLabel sixtyFourDetails;
  QVBoxLayout layout;
  NewestImageUrl newestImageUrl;
  bool hasError;
};

#endif  // SRC_IMAGE_SELECT_PAGE_H_
