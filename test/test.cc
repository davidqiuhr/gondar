#include "test.h"

#include <algorithm>

#include "deviceguy.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

namespace gondar {

void Test::deviceLessThan() {
  QVERIFY(Device("a", Device::Id(0)) < Device("b", Device::Id(0)));
  QVERIFY(Device("a", Device::Id(0)) < Device("a", Device::Id(1)));
  QVERIFY(Device("a", Device::Id(1)) < Device("b", Device::Id(0)));
  QVERIFY(Device("stubdevice0", Device::Id(0)) <
          Device("stubdevice1", Device::Id(1)));
  std::vector<Device> vec = {
      {Device("b", Device::Id(1)), Device("a", Device::Id(0))}};
  std::sort(vec.begin(), vec.end());
  QCOMPARE(vec.front(), Device("a", Device::Id(0)));
  QCOMPARE(vec.back(), Device("b", Device::Id(1)));
}
}

QTEST_MAIN(gondar::Test)
