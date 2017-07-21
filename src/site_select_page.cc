#include "site_select_page.h"

#include "gondarsite.h"
#include "gondarwizard.h"

SiteSelectPage::SiteSelectPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Site Select");
  setSubTitle(
      "Your account is associated with more than one site. "
      "Select the site for which you would like to burn an image.");
  setLayout(&layout);
}

void SiteSelectPage::initializePage() {
  QList<GondarSite*>* sitesList = &wizard()->chromeoverLoginPage.siteList;
  for (int i = 0; i < sitesList->size(); i++) {
    GondarSite* curButton = sitesList->at(i);
    sitesButtons.addButton(curButton);
    layout.addWidget(curButton);
  }
}

bool SiteSelectPage::validatePage() {
  // if we have a site selected, update our download links and continue
  // otherwise, return false
  GondarSite* selected =
      dynamic_cast<GondarSite*>(sitesButtons.checkedButton());
  if (selected == NULL) {
    return false;
  } else {
    // if we have a site selected, update our download links for that
    // go find the sitesList
    qDebug() << "DEBUG: url64 selected=" << selected->get64Url();
    qDebug() << "DEBUG: url32 selected=" << selected->get32Url();
    wizard()->imageSelectPage.set64Url(selected->get64Url());
    wizard()->imageSelectPage.set32Url(selected->get32Url());
    return true;
  }
}
