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

#include <QNetworkReply>
#include <QProgressBar>
#include <QtWidgets>

#include "diskwritethread.h"
#include "downloader.h"
#include "unzipthread.h"

#include "device.h"
#include "gondar.h"
#include "log.h"
#include "metric.h"
#include "neverware_unzipper.h"

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

UsbInsertPage::UsbInsertPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Please insert an 8GB or 16GB USB storage device");
  setSubTitle(
      "In the next step, the selected device will be permanantly erased and "
      "turned into a CloudReady installer.");

  label.setText(
      "Sandisk devices are not recommended.  "
      "Devices with more than 16GB of space may be unreliable.  "
      "The next screen will become available once a valid "
      "destination drive is detected.");
  label.setWordWrap(true);

  layout.addWidget(&label);
  setLayout(&layout);
}

void UsbInsertPage::initializePage() {
  connect(&tim, &QTimer::timeout, this, &UsbInsertPage::getDriveList);
  getDriveList();
}

bool UsbInsertPage::isComplete() const {
  // Page is not complete until the device picker has at least one
  // device available
  return !wizard()->devicePicker()->isEmpty();
}

void UsbInsertPage::getDriveList() {
  auto* picker = wizard()->devicePicker();

  picker->refresh();

  for (const auto& device : picker->allDevices()) {
    LOG_INFO << "Device(id: " << device.device_num << ", name: " << device.name
             << ")";
  }

  if (picker->isEmpty()) {
    tim.start(1000);
  } else {
    tim.stop();
    emit completeChanged();
  }
}

DeviceSelectPage::DeviceSelectPage(QWidget* parent) : WizardPage(parent) {
  // this page should just say 'hi how are you' while it stealthily loads
  // the usb device list.  or it could ask you to insert your device
  setTitle("USB device selection");
  setSubTitle("Choose your target device from the list of devices below.");

  label_.setText("Select Drive:");
  layout_.addWidget(&label_);
  layout_.addWidget(&device_picker_);
  setLayout(&layout_);
}

void DeviceSelectPage::initializePage() {
  device_picker_.refresh();
}

bool DeviceSelectPage::validatePage() {
  return device_picker_.hasSelection();
}

int DeviceSelectPage::nextId() const {
  if (wizard()->downloadProgressPage.isComplete()) {
    return GondarWizard::Page_writeOperation;
  } else {
    return GondarWizard::Page_downloadProgress;
  }
}

WriteOperationPage::WriteOperationPage(QWidget* parent) : WizardPage(parent) {
  layout.addWidget(&progress);
  setLayout(&layout);
}

void WriteOperationPage::initializePage() {
  // set the titles in initializePage for 'make another' flow
  setTitle("Creating your CloudReady USB installer");
  setSubTitle("This process may take up to 20 minutes.");
  writeFinished = false;
  // what if we just start writing as soon as we get here
  if (selected_drive == NULL) {
    qDebug() << "ERROR: no drive selected";
  } else {
    writeToDrive();
  }
}

bool WriteOperationPage::isComplete() const {
  return writeFinished;
}

bool WriteOperationPage::validatePage() {
  return writeFinished;
}

void WriteOperationPage::writeToDrive() {
  qDebug() << "Writing to drive...";
  image_path.clear();
  image_path.append(wizard()->downloadProgressPage.getImageFileName());
  showProgress();
  diskWriteThread = new DiskWriteThread(selected_drive, image_path, this);
  connect(diskWriteThread, SIGNAL(finished()), this, SLOT(onDoneWriting()));
  qDebug() << "launching thread...";
  gondar::SendMetric(gondar::Metric::UsbAttempt);
  diskWriteThread->start();
}

void WriteOperationPage::showProgress() {
  progress.setRange(0, 0);
  progress.setValue(0);
}

void WriteOperationPage::onDoneWriting() {
  switch (diskWriteThread->state()) {
    case DiskWriteThread::State::Initial:
    case DiskWriteThread::State::Running:
      // It should not be possible to get here at runtime
      writeFailed("Internal state error");
      return;

    case DiskWriteThread::State::GetFileSizeFailed:
      writeFailed("Error reading the disk image's file size");
      return;

    case DiskWriteThread::State::InstallFailed:
      writeFailed("Error writing to the USB device");
      return;

    case DiskWriteThread::State::Success:
      // Hooray!
      gondar::SendMetric(gondar::Metric::UsbSuccess);
      break;
  }

  setTitle("CloudReady USB created!");
  setSubTitle("You may now either exit or create another USB.");
  qDebug() << "install call returned";
  writeFinished = true;
  progress.setRange(0, 100);
  progress.setValue(100);
  wizard()->setMakeAnotherLayout();
  // when a USB was successfully created, report time the run took
  gondar::SendMetric(gondar::Metric::SuccessDuration,
                     QString::number(wizard()->getRunTime()).toStdString());
  emit completeChanged();
}

// though error page follows in index, this is the end of the wizard for
// healthy flows
int WriteOperationPage::nextId() const {
  return -1;
}

void WriteOperationPage::writeFailed(const QString& errorMessage) {
  wizard()->postError(errorMessage);
  writeFinished = true;
  emit completeChanged();
}
