
#ifndef GONDAR_METRIC_H
#define GONDAR_METRIC_H

#include <QNetworkAccessManager>
#include <QStringList>
#include <string>

namespace gondar {

enum class Metric {Use};

QNetworkAccessManager* getNetworkManager();
std::string getMetricString(Metric metric);
void SendMetric(Metric metric);

}  // namespace gondar

#endif /* GONDAR_METRIC_H */
