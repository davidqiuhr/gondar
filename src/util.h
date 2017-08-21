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

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <algorithm>

#include <QString>

namespace gondar {

// Read the contents of |filepath| and decode as UTF-8. Throws an
// exception on failure.
QString readUtf8File(const QString& filepath);
bool isChromeover();

// Shortcut for common use of std::find
template <typename Container, typename Key>
bool contains(const Container& container, const Key& key) {
  const auto end = container.end();
  return std::find(container.begin(), end, key) != end;
}

}  // namespace gondar

#endif  // SRC_UTIL_H_
