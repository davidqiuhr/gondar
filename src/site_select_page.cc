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

#include "gondarsite.h"
#include "gondarwizard.h"
#include "log.h"
#include "metric.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    findLabel.setText("Search:");
    findButton.setText("Find");
    findText = "";
    layout.addWidget(&findLabel);
    layout.addWidget(&lineEdit);
    layout.addWidget(&findButton);
    setLayout(&layout);
    setWindowTitle(tr("Find a Contact"));
}

/*
void FindDialog::findClicked() {
    QString text = lineEdit.text();

    if (text.isEmpty()) {
        LOG_WARNING << "field is empty";
        return;
    } else {
        findText = text;
        lineEdit.clear();
        //hide();
    }
}
*/

QString FindDialog::getFindText()
{
    return findText;
}

class SiteEntry : public QListWidgetItem {
 public:
  explicit SiteEntry(const GondarSite& site) : site_(site) {
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
  layout.addWidget(&find);
  layout.addWidget(&sitesEntries);
}

void SiteSelectPage::initializePage() {
  const auto& sitesList = wizard()->sites();
  for (const auto& site : sitesList) {
    auto* curSite = new SiteEntry(site);
    sitesEntries.addItem(curSite);
  }
  connect(&find.findButton, &QPushButton::clicked,
          this, &SiteSelectPage::filterSites);
}

bool SiteSelectPage::validatePage() {
  // if we have a site selected, update our download links and continue
  // otherwise, return false
  auto* selected = dynamic_cast<SiteEntry*>(sitesEntries.currentItem());
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

void SiteSelectPage::filterSites() {
  LOG_WARNING << "clicked";
}
