
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QUrl * url_in, QObject *parent)
    : QThread(parent) {
    url = * url_in;
}

UnzipThread::~UnzipThread() {
}

void UnzipThread::run() {
    qDebug() << "running unzip on url=" << url;
    const char * url_c_str = url.toString().toStdString().c_str();
    neverware_unzip(url_c_str);
    qDebug() << "worker thread says complete";
    emit complete();
}
