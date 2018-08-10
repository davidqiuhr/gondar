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

#include <QApplication>
#include <QLibraryInfo>
#include <QtPlugin>

#include "gondarwizard.h"
#include "log.h"
#include "metric.h"
#include "util.h"

void sleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

int main(int argc, char* argv[]) {
  gondar::InitializeLogging();
  // metric whistler
  gondar::SendMetric(gondar::Metric::Use);
  // easier than bothering to make this actually async as it's just for testing
  sleep(5000);
  return 0;
}
