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

#include <QAbstractButton>
#include <QPushButton>
#include <QTest>
#include <QWizard>

#include "test.h"

#include "src/device_picker.h"
#include "src/gondarwizard.h"
#include "src/device_select_page.h"

#include "src/log.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

inline void initResource() {

  Q_INIT_RESOURCE(gondarwizard);
}

namespace gondar {

namespace {

QAbstractButton* getDevicePickerButton(DevicePicker* picker, const int index) {
  auto* widget = picker->layout()->itemAt(index)->widget();
  return dynamic_cast<QAbstractButton*>(widget);
}

}  // namespace

uint64_t getValidDiskSize() {
    const uint64_t gigabyte = 1073741824LL;
    return 10 * gigabyte;
}

void Test::testDevicePicker() {
  DevicePicker picker;
  QVERIFY(picker.selectedDevice() == nullopt);

  // Add a single device, does not get auto selected
  picker.refresh({DeviceGuy(1, "a", getValidDiskSize())});
  QVERIFY(picker.selectedDevice() == nullopt);

  // Select the first device
  getDevicePickerButton(&picker, 0)->click();
  QCOMPARE(*picker.selectedDevice(), DeviceGuy(1, "a", getValidDiskSize()));

  // Replace with two new devices
  picker.refresh({DeviceGuy(2, "b", getValidDiskSize()), DeviceGuy(3, "c", getValidDiskSize())});
  QVERIFY(picker.selectedDevice() == nullopt);

  // Select the last device
  auto* btn = getDevicePickerButton(&picker, 1);
  btn->click();

  QCOMPARE(*picker.selectedDevice(), DeviceGuy(3, "c", getValidDiskSize()));
}

// just wait 3 seconds and hit the 'next' button
void proceed(GondarWizard * wizard) {
  // oh i see.  the last arg is a wait in MILLIseconds
  QTest::mouseClick(wizard->button(QWizard::NextButton), Qt::LeftButton, Qt::NoModifier, QPoint(), 3000);
  LOG_WARNING << "id after click=" << wizard->currentId();
}

// an integration test for a simple linux flow wherein the user finishes
// the wizard one time
void Test::testLinuxStubFlow() {
  initResource();
  gondar::InitializeLogging();
  GondarWizard wizard;
  wizard.show();
  // 0->3
  proceed(& wizard);
  // 3->4
  proceed(& wizard);
  // 4->5
  proceed(& wizard);
  // on 5 (deviceselect, we must find the first device and click it
  //QAbstractButton* first_device = (static_cast<DeviceSelectPage*>(wizard.page(GondarWizard::Page_deviceSelect)))->getButton(0);
  //LOG_WARNING << "first_device=" << first_device;
  //QTest::mouseClick(first_device, Qt::LeftButton, Qt::NoModifier, QPoint(), 3000);
  proceed(& wizard);
}
}  // namespace gondar
QTEST_MAIN(gondar::Test)
