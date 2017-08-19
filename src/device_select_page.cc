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

#include "device_select_page.h"

#include <QButtonGroup>
#include <QVBoxLayout>

#include "gondarwizard.h"
#include "log.h"

GondarButton::GondarButton(const QString& text,
                           unsigned int device_num,
                           QWidget* parent)
    : QRadioButton(text, parent) {
  index = device_num;
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
  for (const auto& device : wizard()->usbInsertPage.devices()) {
    // FIXME(kendall): clean these up
    GondarButton* curRadio = new GondarButton(
        QString::fromStdString(device.name), device.device_num, this);
    radioGroup->addButton(curRadio);
    layout->addWidget(curRadio);
  }
  setLayout(layout);
}

bool DeviceSelectPage::validatePage() {
  return selectedDevice() != gondar::nullopt;
}

gondar::Option<DeviceGuy> DeviceSelectPage::selectedDevice() const {
  GondarButton* selected =
      dynamic_cast<GondarButton*>(radioGroup->checkedButton());
  if (selected) {
    try {
      const auto device =
          findDevice(wizard()->usbInsertPage.devices(), selected->index);
      wizard()->writeOperationPage.setDevice(device);
      return device;
    } catch (const std::runtime_error& error) {
      LOG_ERROR << error.what();
    }
  }

  return gondar::nullopt;
}

int DeviceSelectPage::nextId() const {
  if (wizard()->downloadProgressPage.isComplete()) {
    return GondarWizard::Page_writeOperation;
  } else {
    return GondarWizard::Page_downloadProgress;
  }
}
