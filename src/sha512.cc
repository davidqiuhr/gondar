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

#include <stdexcept>

#include <QCryptographicHash>
#include <QFile>

#include "log.h"
#include "sha512.h"

namespace gondar {

Option<Sha512> Sha512::calculateFileHash(const QString& filePath) {
  QFile file(filePath);
  if (!file.open(QFile::ReadOnly)) {
    LOG_ERROR << "failed to open " << filePath << ": " << file.errorString();
    return nullopt;
  }

  QCryptographicHash hasher(QCryptographicHash::Sha512);
  if (!hasher.addData(&file)) {
    LOG_ERROR << "failed to hash " << filePath;
    return nullopt;
  }

  return fromBytes(hasher.result());
}

Option<Sha512> Sha512::fromBytes(const QByteArray& bytes) {
  const auto num_expected_bytes = 64;
  if (bytes.size() == num_expected_bytes) {
    return Sha512(bytes);
  } else {
    LOG_ERROR << "invalid hash length: " << bytes.size();
    return nullopt;
  }
}

Option<Sha512> Sha512::fromHex(const QString& str) {
  return fromBytes(QByteArray::fromHex(str.toUtf8()));
}

bool Sha512::operator==(const Sha512& other) const {
  return bytes_ == other.bytes_;
}

bool Sha512::operator!=(const Sha512& other) const {
  return bytes_ != other.bytes_;
}

Sha512::Sha512(const QByteArray& bytes) : bytes_(bytes) {}

}  // namespace gondar
