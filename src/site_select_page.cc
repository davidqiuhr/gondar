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
#include "log.h"
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

void SiteSelectPage::updateSitesForPage() {
  // lowest # site to display
  int min = (page - 1) * 5;
  // first site we do not display
  int max = page * 5;
  int itr = 0;
  // assumes sitesButtons has already been populated
  for (const auto &button : sitesButtons.buttons()) {
    if (itr >= min && itr < max) {
      // ideally we would not instantiate the sitebuttons.
      // we will need a separate vector of sitebuttons
      // and then we will add/remove them from sitesButtons list?
      // or we just toggle them invisible?  that seems simpler
      button->setVisible(true);
    } else {
      button->setVisible(false);
    }
    itr++;
  }
  pageGroup.setTitle(QString("Page %1 / %2").arg(page).arg(getTotalPages()));
}

int SiteSelectPage::getTotalPages() {
  int total_sites = wizard()->sites().size();
  // arithmetic wiggle is a little weird here, but works out
  // s.t. 1 site = 1 page, 5 sites = 1 page, 6 sites = 2 pages
  int total_pages = (total_sites + 4) / 5;
  return total_pages;
}

void SiteSelectPage::initializePage() {
  const auto& sitesList = wizard()->sites();
  for (const auto& site : sitesList) {
    auto* curButton = new SiteButton(site);
    sitesButtons.addButton(curButton);
    layout.addWidget(curButton);
  }
  page = 1;
  bool lots_of_sites = false;
  if (wizard()->sites().size() > 5) {
    lots_of_sites = true;
  }
  pageGroup.setVisible(lots_of_sites);
  prevPageButton.setText("Previous Page");
  nextPageButton.setText("Next Page");
  layout.addWidget(&pageGroup);
  pageNavLayout.addWidget(&prevPageButton);
  pageNavLayout.addWidget(&nextPageButton);
  pageGroup.setLayout(&pageNavLayout);
  connect(&nextPageButton, &QPushButton::clicked, this,
          &SiteSelectPage::handleNextPage);
  connect(&prevPageButton, &QPushButton::clicked, this,
          &SiteSelectPage::handlePrevPage);
  // limit visible sites to those on this page
  updateSitesForPage();
}

void SiteSelectPage::handleNextPage() {
  if (page < getTotalPages()) {
    page++;
    updateSitesForPage();
  }
}

void SiteSelectPage::handlePrevPage() {
  if (page > 1) {
    page--;
    updateSitesForPage();
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
    // metrics may now include site id
    gondar::SetSiteId(site.getSiteId());
    QList<GondarImage> imageList = site.getImages();
    wizard()->imageSelectPage.addImages(imageList);
    return true;
  }
}
