
#ifndef BARATHRUM_H
#define BARATHRUM_H

#include <QNetworkAccessManager>

class Barathrum {
 public:
  static Barathrum& getInstance() {
    static Barathrum instance;
    return instance;
  }
  QNetworkAccessManager* getManager() { return &manager;}
 private:
  QNetworkAccessManager manager;
  Barathrum() {};
 public:
  Barathrum(Barathrum const&) = delete;
  void operator=(Barathrum const&) = delete;
};

#endif /* BARATHRUM_H */
