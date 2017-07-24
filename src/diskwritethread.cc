
#include "diskwritethread.h"

#include <QFile>

#include "device.h"
#include "gondar.h"
#include "log.h"
#include "metric.h"

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

DiskWriteThread::State DiskWriteThread::state() const {
  QMutexLocker locker(&state_mutex_);
  return state_;
}

void DiskWriteThread::run() {
  LOG_INFO << "writing " << image_path << " to disk";
  gondar::SendMetric("usbAttempt");
  setState(State::Running);

  const int64_t image_size = getFileSize(image_path);
  if (image_size == -1) {
    LOG_ERROR << "getFileSize failed";
    setState(State::GetFileSizeFailed);
    return;
  }

  if (!Install(&selected_drive, image_path.toStdString().c_str(), image_size)) {
    LOG_ERROR << "Install failed";
    setState(State::InstallFailed);
    return;
  }
  gondar::SendMetric("usbSuccess");
}

void DiskWriteThread::setState(const State state) {
  QMutexLocker locker(&state_mutex_);
  state_ = state;
}
