
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QUrl* url_in,
                         char* filename_in,
                         QObject* parent)
    : QThread(parent) {
  url = *url_in;
  filename = filename_in;
}

UnzipThread::~UnzipThread() {}

void UnzipThread::run() {
  const char* url_c_str = url.toString().toStdString().c_str();
  char * binfile_name = neverware_unzip(url_c_str);
  strcpy(filename, binfile_name);
  delete binfile_name;
  qDebug() << "worker thread says complete";
}
