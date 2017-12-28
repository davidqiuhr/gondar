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

#include <string>
#include <vector>

#include "gondarsite.h"

namespace gondar {

enum class Metric {
  Use,
  UsbAttempt,
  UsbSuccess,
  DownloadAttempt,
  DownloadSuccess,
  DownloadFailure,
  ChromeoverUse,
  BeeroverUse,
  SuccessDuration,
  Error
};

// TODO: i want to set up some kind of statefulness here.  we should be able
// to pass the metric land a site id, and it should just remember it and
// include it from then on
// WIP
void SetSiteId(const std::vector<GondarSite>& sites);

void SendMetric(Metric metric, const std::string& value = "");

}  // namespace gondar

#endif /* GONDAR_METRIC_H */
