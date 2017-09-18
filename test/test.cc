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
#include <QTest>
#include <QWizard>

#include "test.h"

#include "src/device_picker.h"
#include "src/gondarwizard.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

static void initResources() {
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

void Test::initTestCase() {
  initResources();
}

void Test::testDevicePicker() {
  DevicePicker picker;
  QVERIFY(picker.selectedDevice() == nullopt);

  // Add a single device, does not get auto selected
  picker.refresh({DeviceGuy(1, "a")});
  QVERIFY(picker.selectedDevice() == nullopt);

  // Select the first device
  getDevicePickerButton(&picker, 0)->click();
  QCOMPARE(*picker.selectedDevice(), DeviceGuy(1, "a"));

  // Replace with two new devices
  picker.refresh({DeviceGuy(2, "b"), DeviceGuy(3, "c")});
  QVERIFY(picker.selectedDevice() == nullopt);

  // Select the last device
  auto* btn = getDevicePickerButton(&picker, 1);
  btn->click();

  QCOMPARE(*picker.selectedDevice(), DeviceGuy(3, "c"));
}

// an integration test for a simple linux flow wherein the user finishes
// the wizard one time
void Test::testLinuxStubFlow() {
  //Q_INIT_RESOURCE(gondarwizard);
  GondarWizard wizard;
  //wizard.show();
  //QTest::mouseClick(wizard.button(QWizard::NextButton), Qt::LeftButton, Qt::NoModifier, QPoint(), 3);
}
}  // namespace gondar
QTEST_MAIN(gondar::Test)
