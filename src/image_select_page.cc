#include "image_select_page.h"

#include "gondarwizard.h"
#include "log.h"
#include "util.h"

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
}

void ImageSelectPage::initializePage() {
  if (!gondar::isChromeover()) {
    // for beerover, we'll have to check what the latest release is
    newestImageUrl.fetch();
  }
}

bool ImageSelectPage::validatePage() {
  // we only need to prevent proceeding to next page in the beerover case
  if (gondar::isChromeover()) {
    return true;
  } else {
    return newestImageUrl.isReady();
  }
}

int ImageSelectPage::nextId() const {
  return GondarWizard::Page_usbInsert;
}

void ImageSelectPage::set32Url(QUrl url_in) {
  newestImageUrl.set32Url(url_in);
}

void ImageSelectPage::set64Url(QUrl url_in) {
  newestImageUrl.set64Url(url_in);
}

QUrl ImageSelectPage::getUrl() {
  QAbstractButton* selected = bitnessButtons.checkedButton();
  if (selected == &thirtyTwo) {
    return newestImageUrl.get32Url();
  } else if (selected == &sixtyFour) {
    return newestImageUrl.get64Url();
  } else {
    // TODO: decide what this behavior should be
    return newestImageUrl.get64Url();
  }
}
