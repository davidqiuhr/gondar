#include "chromeover_login_page.h"

#include "gondarsite.h"
#include "gondarwizard.h"
#include "log.h"

ChromeoverLoginPage::ChromeoverLoginPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Login");
  setSubTitle("Enter credentials for my.neverware.com.");
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
  meanWordsLabel.setText("That's not right!  Try again.");
  // give it the smithforrestr treatment
  meanWordsLabel.setStyleSheet("QLabel { color : red; }");
  layout.addWidget(&passwordLineEditLabel, 1, 0);
  layout.addWidget(&passwordLineEdit, 1, 1);
  layout.addWidget(&meanWordsLabel, 2, 0);
  meanWordsLabel.setVisible(false);
  setLayout(&layout);
  outstandingSites = 0;
  // don't allow progressing to next page yet
  finished = false;
  // don't allow another launch of server interaction while another is running
  started = false;

  connect(&meepo_, &gondar::Meepo::finished, this,
          &ChromeoverLoginPage::handleMeepoFinished);
}

int ChromeoverLoginPage::nextId() const {
  if (siteList.size() == 0) {
    return GondarWizard::Page_error;
  } else if (siteList.size() > 1) {
    // if there are multiple sites, we'll set the urls on the site select page
    return GondarWizard::Page_siteSelect;
  } else {
    // otherwise, we can skip that step and update the urls here
    wizard()->imageSelectPage.set64Url(siteList[0].get64Url());
    wizard()->imageSelectPage.set32Url(siteList[0].get32Url());
    return GondarWizard::Page_imageSelect;
  }
}

bool ChromeoverLoginPage::validatePage() {
  QAuthenticator auth;
  auth.setUser(usernameLineEdit.text());
  auth.setPassword(passwordLineEdit.text());
  if (finished) {
    return true;
  }
  if (!started) {
    meepo_.start(auth);
  }
  started = true;
  return false;
}

void ChromeoverLoginPage::handleMeepoFinished() {
  if (meepo_.error().isEmpty()) {
    siteList = meepo_.sites();

    // we don't want users to be able to pass through the screen by pressing
    // next while processing.  this will make validatePage pass and immediately
    // move the user on to the next screen
    finished = true;
    // TODO: let's make this a commit page so we don't have to worry about
    // the case of a user revisiting this segment for now
    wizard()->next();
  } else {
    started = false;
    meanWordsLabel.setVisible(true);
  }
}
