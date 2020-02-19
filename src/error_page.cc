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

#include "error_page.h"
#include "gondarwizard.h"

ErrorPage::ErrorPage(QWidget* parent) : WizardPage(parent) {
  setTitle("An error has occurred");
  setSubTitle(" ");
  layout.addWidget(&label);
  label.setText("");
  // we can set the label so it may display links; it's ok
  // if we don't use the link functionality in all cases
  label.setTextFormat(Qt::RichText);
  label.setTextInteractionFlags(Qt::TextBrowserInteraction);
  label.setOpenExternalLinks(true);
  label.setWordWrap(true);
  setLayout(&layout);
}

void ErrorPage::setErrorString(const QString& errorStringIn) {
  label.setText(errorStringIn);
}

bool ErrorPage::errorEmpty() const {
  return label.text().isEmpty();
}

int ErrorPage::nextId() const {
  return -1;
}

void ErrorPage::setVisible(bool visible) {
  WizardPage::setVisible(visible);
  if (visible) {
    // this is never set back; assumes error page is last page to be shown
    setButtonText(QWizard::FinishButton, "Exit");
  }
}
