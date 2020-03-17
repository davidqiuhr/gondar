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

#include "site_select_page.h"

#include <QLabel>
#include <QRadioButton>

#include "gondarsite.h"
#include "gondarwizard.h"
#include "metric.h"

class SiteButton : public QRadioButton {
 public:
  explicit SiteButton(const GondarSite& site) : site_(site) {
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
      "Select the site you'd like to use.");
  setLayout(&layout);
}

void SiteSelectPage::initializePage() {
  const auto& sitesList = wizard()->sites();
  for (const auto& site : sitesList) {
    auto* curButton = new SiteButton(site);
    sitesButtons.addButton(curButton);
    layout.addWidget(curButton);
  }
  // TODO(ken): revise logic so this only shows if there are more pages in
  // either direction?  also change button to have previous and next pages?
  page = 0;
  bool lots_of_sites = false;
  if (wizard()->sites().size() > 5) {
    lots_of_sites = true;
  }
  // FIXME(ken): temp
  lots_of_sites = true;
  moreSitesLabel.setVisible(lots_of_sites);
  moreSitesButton.setVisible(lots_of_sites);
  lessSitesButton.setVisible(lots_of_sites);
  moreSitesLabel.setText("You have a lot of sites!");
  lessSitesButton.setText("Less");
  moreSitesButton.setText("More");
  layout.addWidget(&moreSitesLabel);
  pageNavLayout.addWidget(&lessSitesButton);
  pageNavLayout.addWidget(&moreSitesButton);
  layout.addLayout(&pageNavLayout);
}

bool SiteSelectPage::validatePage() {
  // if we have a site selected, update our download links and continue
  // otherwise, return false
  auto* selected = dynamic_cast<SiteButton*>(sitesButtons.checkedButton());
  if (selected == NULL) {
    return false;
  } else {
    const auto site = selected->site();
    // metrics may now include site id
    gondar::SetSiteId(site.getSiteId());
    QList<GondarImage> imageList = site.getImages();
    wizard()->imageSelectPage.addImages(imageList);
    return true;
  }
}
