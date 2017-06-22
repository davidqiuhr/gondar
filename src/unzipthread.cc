
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QUrl* url_in, QObject* parent, char * filename_in)
    : QThread(parent) {
  url = *url_in;
  filename = filename_in;
}

UnzipThread::~UnzipThread() {}

void UnzipThread::run() {
  const char* url_c_str = url.toString().toStdString().c_str();
  strcpy(filename, neverware_unzip(url_c_str));
  qDebug() << "worker thread says complete";
}
