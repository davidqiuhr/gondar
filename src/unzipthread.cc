
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QUrl* url_in,
                         QObject* parent)
    : QThread(parent) {
  url = *url_in;
}

UnzipThread::~UnzipThread() {}

QString * UnzipThread::getFilename() {
  return & filename;
}
void UnzipThread::run() {
  const char* url_c_str = url.toString().toStdString().c_str();
  const char * binfile_name = neverware_unzip(url_c_str);
  filename.clear();
  filename.append(binfile_name);
  delete binfile_name;
  qDebug() << "worker thread says complete";
}
