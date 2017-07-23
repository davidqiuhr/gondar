// Copyright 2017 Neverware

#include "wizard_page.h"

#include "gondarwizard.h"
#include "widget_utils.h"

namespace gondar {

WizardPage::WizardPage(QWidget* parent) : QWizardPage(parent) {
  layout_.addWidget(&header_);
  layout_.addWidget(&center_, 1);
  setEmptyMargins(&layout_);
  setContentsMargins(0, 0, 0, 0);
  QWizardPage::setLayout(&layout_);
}

void WizardPage::initializePage() {
  // Allow the header to bleed to the edges of the dialog
  parentWidget()->setContentsMargins(0, 0, 0, 0);
}

GondarWizard* WizardPage::wizard() const {
  QWizard* base_wiz = QWizardPage::wizard();
  Q_ASSERT(base_wiz);
  GondarWizard* gee_wiz = dynamic_cast<GondarWizard*>(base_wiz);
  Q_ASSERT(gee_wiz);
  return gee_wiz;
}

void WizardPage::setLayout(QLayout* layout) {
  center_.setLayout(layout);
}

}
