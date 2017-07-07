#include "chromeover_login_page.h"

#include "gondarwizard.h"

ChromeoverLoginPage::ChromeoverLoginPage(QWidget* parent) : QWizardPage(parent) {
  setTitle("Login");
  setSubTitle(
      "Enter credentials for my.neverware.com.");
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));
  // configure username fields
  usernameLineEditLabel.setBuddy(&usernameLineEdit);
  usernameLineEditLabel.setText("Username:");
  layout.addWidget(&usernameLineEditLabel, 0, 0);
  layout.addWidget(&usernameLineEdit, 0, 1);
  // configure password fields
  passwordLineEdit.setEchoMode(QLineEdit::Password);
  passwordLineEditLabel.setBuddy(&passwordLineEdit);
  passwordLineEditLabel.setText("Password:");
  layout.addWidget(&passwordLineEditLabel, 1, 0);
  layout.addWidget(&passwordLineEdit, 1, 1);
  setLayout(&layout);
}

int ChromeoverLoginPage::nextId() const {
    return GondarWizard::Page_usbInsert;
}
