#include "test.h"

namespace gondar {

void Test::sanity() {
  QVERIFY(true);
}
}

QTEST_MAIN(gondar::Test)
