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

#ifndef SRC_SHA512_H_
#define SRC_SHA512_H_

#include <QByteArray>

#include "option.h"

namespace gondar {

class Sha512 {
 public:
  static Option<Sha512> calculateFileHash(const QString& filePath);

  static Option<Sha512> fromBytes(const QByteArray& bytes);

  static Option<Sha512> fromHex(const QString& str);

  bool operator==(const Sha512& other) const;
  bool operator!=(const Sha512& other) const;

 private:
  explicit Sha512(const QByteArray& bytes);

  const QByteArray bytes_;
};

}  // namespace gondar

#endif  // SRC_SHA512_H_
