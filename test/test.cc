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

#include <algorithm>

#include "device.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

namespace gondar {

void Test::deviceLessThan() {
  QVERIFY(DeviceGuy(0, "a") < DeviceGuy(0, "b"));
  QVERIFY(DeviceGuy(0, "a") < DeviceGuy(1, "a"));
  QVERIFY(DeviceGuy(1, "a") < DeviceGuy(0, "b"));
  QVERIFY(DeviceGuy(0, "stubdevice0") < DeviceGuy(1, "stubdevice1"));
  std::vector<DeviceGuy> vec = {{DeviceGuy(1, "b"), DeviceGuy(0, "a")}};
  std::sort(vec.begin(), vec.end());
  QCOMPARE(vec.front(), DeviceGuy(0, "a"));
  QCOMPARE(vec.back(), DeviceGuy(1, "b"));
}
}

QTEST_MAIN(gondar::Test)
