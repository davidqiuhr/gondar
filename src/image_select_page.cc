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
    thirtyTwo.setText("32-bit");
    thirtyTwoDetails.setText(
        "<a href=\"https://guide.neverware.com/supported-devices\">Only "
        "intended "
        "for certified models marked '32-bit Only'</a>");
    thirtyTwoDetails.setTextFormat(Qt::RichText);
    thirtyTwoDetails.setTextInteractionFlags(Qt::TextBrowserInteraction);
    thirtyTwoDetails.setOpenExternalLinks(true);
    sixtyFourDetails.setText("Suitable for most computers made after 2007");
    sixtyFour.setText("64-bit (recommended)");
    sixtyFour.setChecked(true);
    bitnessButtons.addButton(&thirtyTwo);
    bitnessButtons.addButton(&sixtyFour);
    layout.addWidget(&sixtyFour);
    layout.addWidget(&sixtyFourDetails);
    layout.addWidget(&thirtyTwo);
    layout.addWidget(&thirtyTwoDetails);
  }

  setLayout(&layout);
}

bool ImageSelectPage::validatePage() {
  if (
      // beerover case
      bitnessButtons.checkedButton() ==
          qobject_cast<QAbstractButton*>(&thirtyTwo) ||
      // chromeover case
      bitnessButtons.checkedButton()->text().contains("32")) {
    QMessageBox confirmBox;
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setWindowTitle("CloudReady USB Maker");
    confirmBox.setText(
        "32-bit CloudReady is not supported on 64-bit machines. Use 32-bit "
        "CloudReady only on hardware that requires it.");
    // counter-intuitive that RejectRole maps to forward, but the roles
    // really just determine button order and are not used for later logic
    QPushButton* backButton =
        confirmBox.addButton("Back", QMessageBox::ActionRole);
    QPushButton* continueButton =
        confirmBox.addButton("Use 32-bit", QMessageBox::RejectRole);
    confirmBox.setEscapeButton(backButton);
    confirmBox.setDefaultButton(continueButton);
    confirmBox.exec();
    if (confirmBox.clickedButton() ==
        qobject_cast<QAbstractButton*>(continueButton)) {
      return true;

    } else {
      return false;
    }
  }
  // currently this is only a concern in the chromeover case, but we would
  // be equally worried were this true in either case
  if (!bitnessButtons.checkedButton()) {
    return false;
  }
  return true;
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
  if (image.is32Bit()) {
    layout.addWidget(&thirtyTwoDetails);
  }
}

void ImageSelectPage::addImages(QList<GondarImage> images) {
  for (const auto& curImage : images) {
    // FIXME: 32-bit images should be last.  For the time being, this must be
    // handled on the Gondar side.
    if (!curImage.is32Bit()) {
      addImage(curImage);
    }
  }
  for (const auto& curImage : images) {
    if (curImage.is32Bit()) {
      addImage(curImage);
    }
  }
}

// this is what is used later in the wizard to find what url should be used
// FIXME(ken): what to do about this?  maybe it is best to keep
// newestImageUrl here and just go to next page once we've finished loading
QUrl ImageSelectPage::getUrl() {
  QAbstractButton* selected = bitnessButtons.checkedButton();
  if (gondar::isChromeover()) {
    // for chromeover, use the download button's url attribute
    DownloadButton* selected_download_button =
        dynamic_cast<DownloadButton*>(selected);
    return selected_download_button->getUrl();
  } else {
    // for beerover, we had to wait on a url lookup and we consult newestImage
    if (selected == &thirtyTwo) {
      return newestImageUrl.get32Url();
    } else if (selected == &sixtyFour) {
      return newestImageUrl.get64Url();
    } else {
      // TODO(kendall): decide what this behavior should be
      return newestImageUrl.get64Url();
    }
  }
}
