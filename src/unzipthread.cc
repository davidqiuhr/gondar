
#include "unzipthread.h"

#include <QtWidgets>

#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QFileInfo& inputFile, QObject* parent)
    : QThread(parent), inputFile(inputFile) {}

UnzipThread::~UnzipThread() {}

const QString& UnzipThread::getFileName() const {
  return filename;
}
void UnzipThread::run() {
  const QFileInfo binfile = neverware_unzip(inputFile);
  filename = binfile.absoluteFilePath();
  qDebug() << "worker thread says complete";
}
