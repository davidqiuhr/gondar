
#include "unzipthread.h"

#include "string.h"

#include <QtWidgets>

extern "C" {
  #include "neverware_unzipper.h"
}

UnzipThread::UnzipThread(QObject *parent)
    : QThread(parent) {
}

UnzipThread::~UnzipThread() {
}

void UnzipThread::setUrl(QString * url_in) {
    qDebug() << "setting url=" << url_in;
    url.clear();
    url.append(url_in);
}

void UnzipThread::run() {
    qDebug() << "running unzip on url=" << url;
    const char * url_c_str = url.toStdString().c_str();
    neverware_unzip(url_c_str);
    qDebug() << "worker thread says complete";
    emit complete();
}
