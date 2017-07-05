
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QFileInfo& inputFile, QObject* parent)
    : QThread(parent), inputFile(inputFile) {
}

UnzipThread::~UnzipThread() {}

const QString& UnzipThread::getFileName() const {
  return filename;
}
void UnzipThread::run() {
  char* binfile_name = neverware_unzip(inputFile);
  filename = binfile_name;
  free(binfile_name);
  qDebug() << "worker thread says complete";
}
