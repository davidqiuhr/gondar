
#include "unzipthread.h"

#include "string.h"

#include <QtWidgets>

extern "C" {
  #include "neverware_unzipper.h"
}

UnzipThread::UnzipThread(QObject *parent)
    : QThread(parent) {
    memset(url, 0, MAX_URL);
}

UnzipThread::~UnzipThread() {
}

void UnzipThread::setUrl(const char * url_in) {
    qDebug() << "setting url=" << url_in;
    strcpy(url, url_in);
}

void UnzipThread::launchThread() {
    if (!isRunning()) {
        start(LowPriority);
    }
}

void UnzipThread::run() {
    qDebug() << "running unzip on url=" << url;
    neverware_unzip(url);
    qDebug() << "worker thread says complete";
    emit complete();
}
