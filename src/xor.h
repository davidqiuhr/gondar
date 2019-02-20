// Copyright 2019 Neverware
//
// The source code in this file was derived from:
//
//     Rufus: The Reliable USB Formatting Utility
//     Copyright © 2011-2016 Pete Batard <pete@akeo.ie>
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

#ifndef SRC_XOR_H_
#define SRC_XOR_H_

#include <QByteArray>
#include <QString>

void printByteArray(QByteArray in);
QByteArray getRand(int len);
QByteArray getByteArrayFromString(QString in);
QByteArray get_hash(QString in, QByteArray hash1);
QString get_string_from_hashes(QString hash1str, QString hash2str);

#endif  //  SRC_XOR_H_
