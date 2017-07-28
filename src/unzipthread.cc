
#include "unzipthread.h"

#include <QtWidgets>

#include "log.h"
#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QFileInfo& input, QObject* parent)
    : QThread(parent), inputFile(input) {}

UnzipThread::~UnzipThread() {}

const QString& UnzipThread::getFileName() const {
  return filename;
}
void UnzipThread::run() {
  try {
    const QFileInfo binfile = neverware_unzip(inputFile);
    filename = binfile.absoluteFilePath();
  } catch (const std::exception& exc) {
    LOG_ERROR << "unzip failed: " << exc.what();
    filename = QString();
  }
  qDebug() << "worker thread says complete";
}
