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

#include "gondarwizard.h"

#include <QTimer>

#include "log.h"
#include "metric.h"

GondarWizard::GondarWizard(QWidget* parent)
    : QWizard(parent), about_shortcut_(QKeySequence::HelpContents, this) {
  // these pages are automatically cleaned up
  // new instances are made whenever navigation moves on to another page
  // according to qt docs
  setPage(Page_adminCheck, &adminCheckPage);
  // chromeoverLogin and imageSelect are alternatives to each other
  // that both progress to usbInsertPage
  setPage(Page_chromeoverLogin, &chromeoverLoginPage);
  setPage(Page_siteSelect, &siteSelectPage);
  setPage(Page_imageSelect, &imageSelectPage);
  setPage(Page_usbInsert, &usbInsertPage);
  setPage(Page_deviceSelect, &deviceSelectPage);
  setPage(Page_downloadProgress, &downloadProgressPage);
  setPage(Page_writeOperation, &writeOperationPage);
  setPage(Page_error, &errorPage);
  setWizardStyle(QWizard::ModernStyle);
  setWindowTitle(tr("CloudReady USB Maker"));
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));

  setButtonText(QWizard::CustomButton1, "Make Another USB");
  setButtonText(QWizard::CustomButton2, "About");
  setNormalLayout();
  // remove '?' button that does not do anything in our current setup
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  connect(&about_shortcut_, &QShortcut::activated, &about_dialog_,
          &gondar::AboutDialog::show);
  connect(this, SIGNAL(customButtonClicked(int)), this,
          SLOT(handleCustomButton(int)));

  runTime = QDateTime::currentDateTime();
}

void GondarWizard::setNormalLayout() {
  QList<QWizard::WizardButton> button_layout;
  button_layout << QWizard::CustomButton2 << QWizard::Stretch
                << QWizard::NextButton << QWizard::FinishButton;
  setButtonLayout(button_layout);
}

void GondarWizard::setMakeAnotherLayout() {
  QList<QWizard::WizardButton> button_layout;
  button_layout << QWizard::CustomButton2 << QWizard::Stretch
                << QWizard::CustomButton1 << QWizard::NextButton
                << QWizard::FinishButton;
  setButtonLayout(button_layout);
}
// handle event when 'make another usb' button pressed
void GondarWizard::handleCustomButton(int buttonIndex) {
  if (buttonIndex == QWizard::CustomButton1) {
    setNormalLayout();
    // works as long as usbInsertPage is not the last page in wizard
    setStartId(usbInsertPage.nextId() - 1);
    restart();
  } else if (buttonIndex == QWizard::CustomButton2) {
    about_dialog_.show();
  } else {
    LOG_ERROR << "Unknown custom button pressed";
  }
}

int GondarWizard::nextId() const {
  if (errorPage.errorEmpty()) {
    return QWizard::nextId();
  } else {
    if (currentId() == Page_error) {
      return -1;
    } else {
      return Page_error;
    }
  }
}

void GondarWizard::postError(const QString& error) {
  QTimer::singleShot(0, this, [=]() { catchError(error); });
}

void GondarWizard::catchError(const QString& error) {
  LOG_ERROR << "displaying error: " << error;
  errorPage.setErrorString(error);
  // TODO: sanitize error string?
  gondar::SendMetric(gondar::Metric::Error, error.toStdString());
  next();
}

qint64 GondarWizard::getRunTime() {
  return runTime.secsTo(QDateTime::currentDateTime());
}
