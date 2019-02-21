// Copyright 2019 Neverware
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

#include "xor.h"

#include <QByteArray>
#include <QList>
#include <QString>
#include <iostream>
#include <random>

void printByteArray(QByteArray in) {
  std::cout << in.toHex().toStdString() << std::endl;
}

QByteArray getRand(int len) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);
  QByteArray output;
  for (int i = 0; i < len; i++) {
    unsigned char cur = dis(gen);
    output.append(cur);
  }
  return output;
}

QByteArray getByteArrayFromString(QString in) {
  QByteArray out;
  out.append(QByteArray::fromHex(in.toLatin1()));
  return out;
}

QByteArray get_hash(QString in, QByteArray hash1) {
  QByteArray input = in.toLatin1();
  QByteArray salt = hash1;
  QByteArray derived;
  for (int i = 0; i < input.length(); i++) {
    derived.append(input.at(i) ^ salt.at(i));
  }
  return derived;
}

// assumes arguments come in as strings and need to be converted to regular
// hex
QString get_string_from_hashes(QString hash1str, QString hash2str) {
  QByteArray hash1 = getByteArrayFromString(QString(hash1str));
  QByteArray hash2 = getByteArrayFromString(QString(hash2str));
  QByteArray output;
  for (int i = 0; i < hash1.length(); i++) {
    output.append(hash1.at(i) ^ hash2.at(i));
  }
  return QString(output);
}
