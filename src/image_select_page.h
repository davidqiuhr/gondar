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

#ifndef IMAGE_SELECT_PAGE_H
#define IMAGE_SELECT_PAGE_H

#include <QButtonGroup>
#include <QNetworkAccessManager>
#include <QRadioButton>
#include <QUrl>
#include <QVBoxLayout>

#include "newest_image_url.h"

#include "wizard_page.h"

class ImageSelectPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  ImageSelectPage(QWidget* parent = 0);
  QUrl getUrl() const;
  int nextId() const override;
  void set32Url(QUrl url_in);
  void set64Url(QUrl url_in);
  QUrl getUrl();
  void handleNewestImageUrlError();

 protected:
  void initializePage() override;
  bool validatePage() override;

 private:
  QButtonGroup bitnessButtons;
  QRadioButton thirtyTwo;
  QRadioButton sixtyFour;
  QVBoxLayout layout;
  NewestImageUrl newestImageUrl;
  bool hasError;
};

#endif
