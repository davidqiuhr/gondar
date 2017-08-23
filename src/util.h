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

#include <QFrame>
#include <QString>

class QLayout;

namespace gondar {

class HorizontalLine : public QFrame {
 public:
  HorizontalLine();
};

// Read the contents of |filepath| and decode as UTF-8. Throws an
// exception on failure.
QString readUtf8File(const QString& filepath);
bool isChromeover();

void setEmptyMargins(QLayout* layout);

void setDefaultMargins(QLayout* layout);

}  // namespace gondar

#endif  // SRC_UTIL_H_
