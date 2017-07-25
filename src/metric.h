
#ifndef GONDAR_METRIC_H
#define GONDAR_METRIC_H

#include <QNetworkAccessManager>
#include <QStringList>
#include <string>

namespace gondar {

enum class Metric {Use};

void SendMetric(Metric metric);

}  // namespace gondar

#endif /* GONDAR_METRIC_H */
