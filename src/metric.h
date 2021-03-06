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

#ifndef SRC_METRIC_H_
#define SRC_METRIC_H_

#include <QString>
#include <string>

#include "gondarwizard.h"

namespace gondar {

enum class Metric {
  BeeroverUse,
  ChromeoverUse,
  DownloadAttempt,
  DownloadFailure,
  DownloadSuccess,
  Error,
  FormatAttempt,
  FormatSuccess,
  SuccessDuration,
  UsbAttempt,
  UsbSuccess,
  Use,
};

// send a metric just to gondar endpoint, not both gondar and meepo
void SendMetricGondar(Metric metric,
                      const std::string& value = "",
                      int site_id = 0);
// send a metric to gondar endpoint and meepo endpoint if we have a session
void SendMetric(GondarWizard* wizard,
                Metric metric,
                const std::string& value = "");

QString GetUuid();
}  // namespace gondar

#endif  // SRC_METRIC_H_
