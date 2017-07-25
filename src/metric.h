
#ifndef GONDAR_METRIC_H
#define GONDAR_METRIC_H

#include <QStringList>
#include <string>

namespace gondar {

enum Metric {Use};

std::string getMetricString(Metric metric);
void SendMetric(Metric metric);

}  // namespace gondar

#endif /* GONDAR_METRIC_H */
