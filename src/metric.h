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

#ifndef GONDAR_METRIC_H
#define GONDAR_METRIC_H

namespace gondar {

enum class Metric {
  Use,
  UsbAttempt,
  UsbSuccess,
  DownloadAttempt,
  DownloadSuccess,
  ChromeoverUse,
  BeeroverUse
};

void SendMetric(Metric metric);

}  // namespace gondar

#endif /* GONDAR_METRIC_H */
