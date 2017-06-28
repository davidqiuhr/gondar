
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

DiskWriteThread::~DiskWriteThread() {}

void DiskWriteThread::run() {
  qDebug() << "running diskwrite on image=" << image_path;
  // we must save the byte array, as otherwise the pointer could become invalid
  // before we try to access it
  QByteArray image_path_bytes = image_path.toLatin1();
  const char* image_path_c_str = image_path_bytes.data();
  Install(&selected_drive, image_path_c_str);
  qDebug() << "worker thread says complete";
}
