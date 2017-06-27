
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QUrl* url_in,
                         QObject* parent)
    : QThread(parent) {
  url = *url_in;
}

UnzipThread::~UnzipThread() {}

const QString& UnzipThread::getFileName() const {
  return filename;
}
void UnzipThread::run() {
  const char* url_c_str = url.toString().toStdString().c_str();
  char * binfile_name = neverware_unzip(url_c_str);
  filename = binfile_name;
  free(binfile_name);
  qDebug() << "worker thread says complete";
}
