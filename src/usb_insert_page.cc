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

#include "usb_insert_page.h"

#include <QTimer>

#include "gondar.h"
#include "log.h"

UsbInsertPage::UsbInsertPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Please insert an 8GB or 16GB USB storage device");
  setSubTitle(
      "In the next step, the selected device will be permanently erased and "
      "turned into a CloudReady installer.");

  label.setText(
      "Sandisk devices are not recommended.  "
      "The next screen will become available once a valid "
      "USB drive is inserted.");
  label.setWordWrap(true);

  layout.addWidget(&label);
  setLayout(&layout);

  // the next button should be grayed out until the user inserts a USB
  connect(this, &UsbInsertPage::driveListRequested, this,
          &UsbInsertPage::getDriveList);
}

const DeviceGuyList& UsbInsertPage::devices() const {
  return drivelist;
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
    LOG_INFO << device;
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
