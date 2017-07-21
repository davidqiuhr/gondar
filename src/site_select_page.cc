#include "site_select_page.h"

#include <QRadioButton>

#include "gondarsite.h"
#include "gondarwizard.h"

class SiteButton : public QRadioButton {
 public:
  SiteButton(const GondarSite& site) : site_(site) {
    setText(site.getSiteName());
  }

  const GondarSite& site() const { return site_; }

 private:
  const GondarSite site_;
};

SiteSelectPage::SiteSelectPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Site Select");
  setSubTitle(
      "Your account is associated with more than one site. "
      "Select the site for which you would like to burn an image.");
  setLayout(&layout);
}

void SiteSelectPage::initializePage() {
  const auto& sitesList = wizard()->chromeoverLoginPage.siteList;
  for (const auto& site : sitesList) {
    auto* curButton = new SiteButton(*site);
    sitesButtons.addButton(curButton);
    layout.addWidget(curButton);
  }
}

bool SiteSelectPage::validatePage() {
  // if we have a site selected, update our download links and continue
  // otherwise, return false
  auto* selected = dynamic_cast<SiteButton*>(sitesButtons.checkedButton());
  if (selected == NULL) {
    return false;
  } else {
    const auto site = selected->site();

    // if we have a site selected, update our download links for that
    // go find the sitesList
    qDebug() << "DEBUG: url64 selected=" << site.get64Url();
    qDebug() << "DEBUG: url32 selected=" << site.get32Url();
    wizard()->imageSelectPage.set64Url(site.get64Url());
    wizard()->imageSelectPage.set32Url(site.get32Url());
    return true;
  }
}
