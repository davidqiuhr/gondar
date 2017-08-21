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

#include "test.h"

#include "device_list_model.h"
#include "device_picker.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

namespace gondar {

void Test::deviceListModelEmpty() {
  DeviceListModel model;
  QCOMPARE(model.rowCount(), 0);
  QCOMPARE(model.columnCount(), 2);
  QVERIFY(model.deviceFromRow(0) == nullopt);
  QVERIFY(!model.data(model.index(0, 0)).isValid());
  QVERIFY(!model.hasDevice(DeviceGuy(123, "abc")));
}

void Test::deviceListModelOneDevice() {
  DeviceListModel model;
  model.addDevice(DeviceGuy(123, "abc"));
  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.columnCount(), 2);
  QCOMPARE(model.deviceFromRow(0).value(), DeviceGuy(123, "abc"));
  QVERIFY(model.data(model.index(0, 0)).isValid());
  QVERIFY(model.data(model.index(0, 1)).isValid());
  QVERIFY(!model.data(model.index(1, 0)).isValid());
  QVERIFY(!model.data(model.index(1, 1)).isValid());
  QVERIFY(model.hasDevice(DeviceGuy(123, "abc")));
  QVERIFY(!model.hasDevice(DeviceGuy(456, "abc")));
}

void Test::deviceListModelRefresh() {
  DeviceListModel model;
  DeviceGuyList devices = {DeviceGuy(123, "abc"), DeviceGuy(456, "def")};

  model.refresh(devices);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.deviceFromRow(0).value(), DeviceGuy(123, "abc"));
  QCOMPARE(model.deviceFromRow(1).value(), DeviceGuy(456, "def"));

  model.refresh(devices);
  QCOMPARE(model.rowCount(), 2);
  QCOMPARE(model.deviceFromRow(0).value(), DeviceGuy(123, "abc"));
  QCOMPARE(model.deviceFromRow(1).value(), DeviceGuy(456, "def"));
}

void Test::devicePickerRefresh() {
  DevicePicker picker;
  DeviceGuyList devices = {DeviceGuy(123, "abc"), DeviceGuy(456, "def")};

  picker.refresh(devices);
  // picker.listView().
}
}

QTEST_MAIN(gondar::Test)
