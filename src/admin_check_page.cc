#include "admin_check_page.h"
#include "gondar.h"
#include "gondarwizard.h"

#include "gondarwizard.h"

// note that even though this is called the admin check page, it will in most
// cases be a welcome page, unless the user is missing admin rights
AdminCheckPage::AdminCheckPage(QWidget* parent) : WizardPage(parent) {
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));

  is_admin = false;  // assume false until we discover otherwise.
                     // this holds the user at this screen

  layout.addWidget(&label);
  setLayout(&layout);
}

void AdminCheckPage::initializePage() {
  is_admin = IsCurrentProcessElevated();
  if (!is_admin) {
    showIsNotAdmin();
  } else {
    showIsAdmin();
  }
  //FIXME(kendall): this currently renders 'jimbo jimbo' beneath 'You will need'
  wizard()->goToErrorPage(QString("jimbo jimbo"));
}

bool AdminCheckPage::isComplete() const {
  // the next button is grayed out if user does not have appropriate rights
  return is_admin;
}

void AdminCheckPage::showIsAdmin() {
  setTitle("Welcome to the CloudReady USB Creation Utility");
  // note that a subtitle must be set  on a page in order for logo to display
  setSubTitle(
      "This utility will create a USB device that can be used to install "
      "CloudReady on any computer.");
  label.setText(
      "<p>You will need:</p><ul><li>8GB or 16GB USB stick</li><li>20 minutes "
      "for USB installer creation</li></ul>");
  label.setWordWrap(true);
  emit completeChanged();
}

void AdminCheckPage::showIsNotAdmin() {
  setTitle("User does not have administrator rights");
  setSubTitle(
      "The current user does not have adminstrator rights or the program was "
      "run without sufficient rights to create a USB.  Please re-run the "
      "program with sufficient rights.");
}

// helper function to determine if this build is a chromeover build
bool AdminCheckPage::isChromeover() {
#ifdef CHROMEOVER
  return true;
#else
  return false;
#endif
}

int AdminCheckPage::nextId() const {
  if (isChromeover()) {
    return GondarWizard::Page_chromeoverLogin;
  } else {
    return GondarWizard::Page_imageSelect;
  }
}
