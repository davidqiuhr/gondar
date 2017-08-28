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

#ifndef LOG_H
#define LOG_H

// Qt headers first to ensure that the plog internal ifdefs enable Qt
// support
#include <QString>
#include <QUrl>

#include "plog/Log.h"

class DeviceGuy;

namespace plog {
void operator<<(util::nstringstream& stream, const DeviceGuy& device);
void operator<<(util::nstringstream& stream, const QUrl& url);
}  // namespace plog

namespace gondar {

void InitializeLogging();

}  // namespace gondar

#endif
