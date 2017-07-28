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
    setButtonText(QWizard::CustomButton1, "Exit");
    wizard()->setOption(QWizard::HaveCustomButton1, true);
    connect(wizard(), SIGNAL(customButtonClicked(int)),
            QApplication::instance(), SLOT(quit()));
  } else {
    wizard()->setOption(QWizard::HaveCustomButton1, false);
    disconnect(wizard(), SIGNAL(customButtonClicked(int)),
               QApplication::instance(), SLOT(quit()));
  }
}
