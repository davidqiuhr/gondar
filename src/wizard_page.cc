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

#include "wizard_page.h"

#include "gondarwizard.h"
#include "util.h"

namespace gondar {

WizardPage::WizardPage(QWidget* parent) : QWidget(parent) {
  layout_.addWidget(&header_);
  layout_.addWidget(&center_, 1);
  setEmptyMargins(&layout_);
  QWidget::setLayout(&layout_);
}

int WizardPage::nextId() const {
  return -1;
}

void WizardPage::setWizard(GondarWizard* wizard) {
  wizard_ = wizard;
}

void WizardPage::initializePage() {
  // Allow the header to bleed to the edges of the dialog
  parentWidget()->setContentsMargins(0, 0, 0, 0);
}

GondarWizard* WizardPage::wizard() const {
  return wizard_;
}

void WizardPage::setLayout(QLayout* layout) {
  setDefaultMargins(layout);
  center_.setLayout(layout);
}

void WizardPage::setButtonText(QWizard::WizardButton button,
                               const QString& text) {}

void WizardPage::setTitle(const QString& text) {
  header_.setTitle(text);
}
void WizardPage::setSubTitle(const QString& text) {
  header_.setSubtitle(text);
}

bool WizardPage::isComplete() const {}
bool WizardPage::validatePage() {}
}
