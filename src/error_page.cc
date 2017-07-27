// Copyright 2017 Neverware

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
