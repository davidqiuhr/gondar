
#include "diskwritethread.h"

#include <QtWidgets>

#include "deviceguy.h"
#include "gondar.h"

static int64_t getFileSize(QString& path) {
  QFile file(path);
  if (!file.exists()) {
    return -1;
  }
  return file.size();
}

DiskWriteThread::DiskWriteThread(DeviceGuy* drive_in,
                                 const QString& image_path_in,
                                 QObject* parent)
    : QThread(parent) {
  DeviceGuy_copy(drive_in, &selected_drive);
  image_path = image_path_in;
}

DiskWriteThread::~DiskWriteThread() {}

void DiskWriteThread::run() {
  qDebug() << "running diskwrite on image=" << image_path;
  int64_t image_size = getFileSize(image_path);
  Install(&selected_drive, image_path.toStdString().c_str(), image_size);
  qDebug() << "worker thread says complete";
}
