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

#include <QAbstractButton>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>

#include "src/device_picker.h"
#include "src/log.h"
#include "src/meepo.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

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
  picker.refresh({DeviceGuy(2, "b", getValidDiskSize()),
                  DeviceGuy(3, "c", getValidDiskSize())});
  QVERIFY(picker.selectedDevice() == nullopt);

  // Select the last device
  auto* btn = getDevicePickerButton(&picker, 1);
  btn->click();

  QCOMPARE(*picker.selectedDevice(), DeviceGuy(3, "c", getValidDiskSize()));
}

void Test::testMeepoGetMetricJson() {
  Meepo meepo;
  meepo.setSiteId(3);
  meepo.setToken(QString("kewltok"));

  QString version = getGondarVersion();
  if (version.length() == 0) {
    version = "none";
  }

  // then we test the json against expected for our meepo's state
  QString expected_json_str =
      QString(
          "{\"activity\":{\"activity\":\"usb-maker-kewlmetric\","
          "\"description\":"
          "\"version=%1,value=kewlvalue\",\"site_id\":3}}")
          .arg(version);
  QJsonDocument expected_doc =
      QJsonDocument::fromJson(expected_json_str.toUtf8());
  // then we test the url against expected for our meepo's state
  QString metric_json = meepo.getMetricJson("kewlmetric", "kewlvalue");
  QString expected = expected_doc.toJson(QJsonDocument::Compact);
  QCOMPARE(metric_json, expected);
}

// currently this just tests that the url is as expected
void Test::testMeepoGetMetricRequest() {
  Meepo meepo;
  meepo.setToken(QString("kewltok2"));
  auto expected_url =
      QUrl("https://api.grv.neverware.com/poof/activity?token=kewltok2");
  QNetworkRequest actual_request = meepo.getMetricRequest();
  QCOMPARE(actual_request.url(), expected_url);
}

}  // namespace gondar

QTEST_MAIN(gondar::Test)
