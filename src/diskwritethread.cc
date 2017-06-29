
#include "diskwritethread.h"

#include <QtWidgets>

#include "deviceguy.h"
#include "gondar.h"

DiskWriteThread::DiskWriteThread(DeviceGuy* drive_in,
                                 const QString& image_path_in,
                                 QObject* parent)
    : QThread(parent) {
  DeviceGuy_copy(drive_in, &selected_drive);
  image_path = image_path_in;
}

int64_t DiskWriteThread::getFileSize(const char* path) {
  QFile file(path);
  if (!file.exists()) {
    return -1;
  }
  return file.size();
}

DiskWriteThread::~DiskWriteThread() {}

void DiskWriteThread::run() {
  qDebug() << "running diskwrite on image=" << image_path;
  // hold on to string in this scope so c string pointer does not invalidate
  std::string image_path_str = image_path.toStdString();
  const char* image_path_c_str = image_path_str.c_str();
  int64_t image_size = getFileSize(image_path_c_str);
  Install(&selected_drive, image_path_c_str, image_size);
  qDebug() << "worker thread says complete";
}
