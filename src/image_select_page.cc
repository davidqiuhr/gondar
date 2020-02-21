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

#include "image_select_page.h"

#include <QMessageBox>
#include <QPushButton>
#include "gondarimage.h"
#include "gondarwizard.h"
#include "log.h"
#include "util.h"

class DownloadButton : public QRadioButton {
 public:
  explicit DownloadButton(QUrl& url_in) : url(url_in) {}
  QUrl getUrl() const { return url; }

 private:
  QUrl url;
};

ImageSelectPage::ImageSelectPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Which version of CloudReady do you need?");
  setSubTitle(" ");

  // we use these buttons for beerover only now
  if (!gondar::isChromeover()) {
    sixtyFourDetails.setText("Suitable for most computers made after 2007");
    sixtyFour.setText("64-bit (recommended)");
    sixtyFour.setChecked(true);
    bitnessButtons.addButton(&sixtyFour);
    layout.addWidget(&sixtyFour);
    layout.addWidget(&sixtyFourDetails);
  }

  setLayout(&layout);
  // FIXME(ken): when does the constructor fire?  is it adequate to put this
  // here?  feels like this should happen at the wizard level.  ideally this
  // page should not have sucvh an intimate relationship with newestImageUrl.
  // instead it should be part of the wizard and pages can access it that way.
  connect(&newestImageUrl, &NewestImageUrl::errorOccurred, this,
          &ImageSelectPage::handleNewestImageUrlError);
}

// if beerover, start the requisite image fetching
void ImageSelectPage::maybe_fetch() {
  if (!gondar::isChromeover()) {
    // for beerover, we'll have to check what the latest release is
    newestImageUrl.fetch();
  }
}

bool ImageSelectPage::validatePage() {
  // if there is an error, we need to allow the user to proceed to the error
  // screen
  if (wizard()->getError()) {
    return true;
  }
  // currently this is only a concern in the chromeover case, but we would
  // be equally worried were this true in either case
  if (!bitnessButtons.checkedButton()) {
    return false;
  }
  return true;
}

bool ImageSelectPage::newestIsReady() {
  return newestImageUrl.isReady();
}

int ImageSelectPage::nextId() const {
  return GondarWizard::Page_usbInsert;
}

void ImageSelectPage::addImage(GondarImage image) {
  // do not list deployable images
  if (image.isDeployable()) {
    return;
  }
  DownloadButton* newButton = new DownloadButton(image.url);
  newButton->setText(image.getCompositeName());
  bitnessButtons.addButton(newButton);
  layout.addWidget(newButton);
}

void ImageSelectPage::addImages(QList<GondarImage> images) {
  for (const auto& curImage : images) {
    addImage(curImage);
  }
}

// this is what is used later in the wizard to find what url should be used
QUrl ImageSelectPage::getUrl() {
  QAbstractButton* selected = bitnessButtons.checkedButton();
  if (gondar::isChromeover()) {
    // for chromeover, use the download button's url attribute
    DownloadButton* selected_download_button =
        dynamic_cast<DownloadButton*>(selected);
    return selected_download_button->getUrl();
  } else {
    // for beerover, we had to wait on a url lookup and we consult newestImage
    if (selected == &sixtyFour) {
      return newestImageUrl.get64Url();
    } else {
      // TODO(kendall): decide what this behavior should be
      return newestImageUrl.get64Url();
    }
  }
}

void ImageSelectPage::handleNewestImageUrlError() {
  // TODO(ken): this should be moved out of this page now? vOv
  wizard()->postError("An error has occurred fetching the latest image");
}
