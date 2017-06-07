
#include "unzipthread.h"

#include "string.h"

#include <QtWidgets>

extern "C" {
  #include "neverware_unzipper.h"
}

UnzipThread::UnzipThread(QString * url_in, QObject *parent)
    : QThread(parent) {
    url.clear();
    url.append(url_in);
}

UnzipThread::~UnzipThread() {
}

void UnzipThread::run() {
    qDebug() << "running unzip on url=" << url;
    const char * url_c_str = url.toStdString().c_str();
    neverware_unzip(url_c_str);
    qDebug() << "worker thread says complete";
    emit complete();
}
