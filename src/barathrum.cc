
#include "barathrum.h"

QNetworkAccessManager* getNetworkManager() {
  static QNetworkAccessManager manager;
  return & manager;
}
