
#ifndef DISKWRITE_THREAD_H
#define DISKWRITE_THREAD_H

#include <QMutex>
#include <QString>
#include <QThread>

#include "deviceguy.h"

class DiskWriteThread : public QThread {
  Q_OBJECT
 public:
  DiskWriteThread(DeviceGuy* drive_in,
                  const QString& image_path_in,
                  QObject* parent = 0);
  ~DiskWriteThread();

  enum class State : uint32_t {
    Initial,
    Running,
    GetFileSizeFailed,
    InstallFailed,
    Success,
  };

  State state() const;

 protected:
  void run() override;

 private:
  void setState(State state);

  mutable QMutex state_mutex_;
  State state_ = State::Initial;

  DeviceGuy selected_drive;
  QString image_path;
};

#endif /* DISKWRITE_THREAD_H */
