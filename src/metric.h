
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
