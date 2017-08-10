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

DeviceGuyList drivelist;

GondarButton::GondarButton(const QString& text,
                           unsigned int device_num,
                           QWidget* parent)
    : QRadioButton(text, parent) {
  index = device_num;
}
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

  // the next button should be grayed out until the user inserts a USB
  connect(this, SIGNAL(driveListRequested()), this, SLOT(getDriveList()));
}

void UsbInsertPage::initializePage() {
  // if the page is visited again, delete the old drivelist
  drivelist.clear();
  // send a signal to check for drives
  emit driveListRequested();
}

bool UsbInsertPage::isComplete() const {
  // this should return false unless we have a non-empty result from
  // GetDevices()
  if (drivelist.empty()) {
    return false;
  } else {
    return true;
  }
}

void UsbInsertPage::getDriveList() {
  drivelist = GetDeviceList();
  for (const auto& device : drivelist) {
    LOG_INFO << "Device(id: " << device.device_num << ", name: " << device.name
             << ")";
  }

  if (drivelist.empty()) {
    const int second_in_milliseconds = 1000;
    QTimer::singleShot(second_in_milliseconds, this,
                       &UsbInsertPage::getDriveList);
  } else {
    showDriveList();
  }
}

void UsbInsertPage::showDriveList() {
  emit completeChanged();
}

DeviceSelectPage::DeviceSelectPage(QWidget* parent) : WizardPage(parent) {
  // this page should just say 'hi how are you' while it stealthily loads
  // the usb device list.  or it could ask you to insert your device
  setTitle("USB device selection");
  setSubTitle("Choose your target device from the list of devices below.");
  layout = new QVBoxLayout;
  drivesLabel.setText("Select Drive:");
  radioGroup = NULL;
  setLayout(layout);
}

void DeviceSelectPage::initializePage() {
  // while our layout is not empty, remove items from it
  while (!layout->isEmpty()) {
    QLayoutItem* curItem = layout->takeAt(0);
    if (curItem->widget() != &drivesLabel) {
      delete curItem->widget();
    }
  }

  // remove our last listing
  delete radioGroup;

  // Line up widgets horizontally
  // use QVBoxLayout for vertically, H for horizontal
  layout->addWidget(&drivesLabel);

  radioGroup = new QButtonGroup();
  // i could extend the button object to also have a secret index
  // then i could look up index later easily
  for (const auto& device : drivelist) {
    // FIXME(kendall): clean these up
    GondarButton* curRadio = new GondarButton(
        QString::fromStdString(device.name), device.device_num, this);
    radioGroup->addButton(curRadio);
    layout->addWidget(curRadio);
  }
  setLayout(layout);
}

bool DeviceSelectPage::validatePage() {
  // TODO(kendall): check for NULL on bad cast
  GondarButton* selected =
      dynamic_cast<GondarButton*>(radioGroup->checkedButton());
  if (selected == NULL) {
    return false;
  } else {
    try {
      const auto device = findDevice(drivelist, selected->index);
      wizard()->writeOperationPage.setDevice(device);
      return true;
    } catch (const std::runtime_error& error) {
      LOG_ERROR << error.what();
      return false;
    }
  }
}

int DeviceSelectPage::nextId() const {
  if (wizard()->downloadProgressPage.isComplete()) {
    return GondarWizard::Page_writeOperation;
  } else {
    return GondarWizard::Page_downloadProgress;
  }
}
