
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
    // returns 0 on file not found
    // TODO: is this guaranteed?
    return QFile(path).size();
}

DiskWriteThread::~DiskWriteThread() {}

void DiskWriteThread::run() {
  qDebug() << "running diskwrite on image=" << image_path;
  std::string image_path_str = image_path.toStdString();
  const char* image_path_c_str = image_path_str.c_str();
  int64_t image_size = getFileSize(image_path_c_str);
  if (image_size == -1) {
    qDebug() << "Error: could not detect image size";
  }
  Install(&selected_drive, image_path_c_str, image_size);
  qDebug() << "worker thread says complete";
}
