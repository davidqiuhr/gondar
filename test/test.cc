#include "test.h"

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

namespace gondar {

void Test::sanity() {
  QVERIFY(true);
}
}

QTEST_MAIN(gondar::Test)
