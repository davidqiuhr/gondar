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

#include "device_picker.h"
#include "gondarwizard.h"
#include "log.h"

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
  refreshDevices();
}

bool UsbInsertPage::isComplete() const {
  // Page becomes complete when the device list is not empty
  return !wizard()->devicePicker()->isEmpty();
}

void UsbInsertPage::refreshDevices() {
  auto* picker = wizard()->devicePicker();
  picker->refresh();

  if (picker->isEmpty()) {
    const int second_in_milliseconds = 1000;
    QTimer::singleShot(second_in_milliseconds, this,
                       &UsbInsertPage::refreshDevices);
  } else {
    emit completeChanged();
  }
}
