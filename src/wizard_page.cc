// Copyright 2017 Neverware

#include "wizard_page.h"
#include "gondarwizard.h"

namespace gondar {

WizardPage::WizardPage(QWidget* parent) : QWizardPage(parent) {}

GondarWizard* WizardPage::wizard() const {
  QWizard* base_wiz = QWizardPage::wizard();
  Q_ASSERT(base_wiz);
  GondarWizard* gee_wiz = dynamic_cast<GondarWizard*>(base_wiz);
  Q_ASSERT(gee_wiz);
  return gee_wiz;
}
}
