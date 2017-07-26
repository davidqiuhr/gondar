// Copyright 2017 Neverware

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <QString>

namespace gondar {

// Read the contents of |filepath| and decode as UTF-8. Throws an
// exception on failure.
QString readUtf8File(const QString& filepath);

}  // namespace gondar

#endif  // SRC_UTIL_H_
