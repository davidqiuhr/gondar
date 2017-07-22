#include "image_select_page.h"

#include "gondarwizard.h"
#include "log.h"

ImageSelectPage::ImageSelectPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Which version of CloudReady do you need?");
  setSubTitle(
      "64-bit should be suitable for most computers made after 2007.  Choose "
      "32-bit for older computers or devices with Intel Atom CPUs.");
  thirtyTwo.setText("32-bit");
  sixtyFour.setText("64-bit (recommended)");
  sixtyFour.setChecked(true);
  bitnessButtons.addButton(&thirtyTwo);
  bitnessButtons.addButton(&sixtyFour);
  layout.addWidget(&thirtyTwo);
  layout.addWidget(&sixtyFour);
  setLayout(&layout);
  thirtyTwoUrl.setUrl(
      "https://ddnynf025unax.cloudfront.net/cloudready-free-56.3.80-32-bit/"
      "cloudready-free-56.3.80-32-bit.bin.zip");
  sixtyFourUrl.setUrl(
      "https://ddnynf025unax.cloudfront.net/cloudready-free-56.3.82-64-bit/"
      "cloudready-free-56.3.82-64-bit.bin.zip");
}

QUrl ImageSelectPage::getUrl() const {
  LOG_INFO << "in getUrl and thirtyTwoUrl=" << thirtyTwoUrl.toString();
  LOG_INFO << "in getUrl and sixtyFourUrl=" << sixtyFourUrl.toString();
  QAbstractButton* selected = bitnessButtons.checkedButton();
  if (selected == &thirtyTwo) {
    return thirtyTwoUrl;
  } else if (selected == &sixtyFour) {
    return sixtyFourUrl;
  } else {
    // TODO: decide what this behavior should be
    return sixtyFourUrl;
  }
}

int ImageSelectPage::nextId() const {
  return GondarWizard::Page_usbInsert;
}

void ImageSelectPage::set32Url(QUrl url_in) {
  thirtyTwoUrl = url_in;
}

void ImageSelectPage::set64Url(QUrl url_in) {
  sixtyFourUrl = url_in;
}
