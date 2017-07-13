
#ifndef DISKWRITE_THREAD_H
#define DISKWRITE_THREAD_H

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

  enum class State {
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
  // When the thread wants to update the |state_| member it cannot do
  // so directly since |state_| is not a thread-safe type. Instead,
  // the thread calls this method which posts an event to the parent
  // thread.
  void setState(State state);

  State state_ = State::Initial;
  DeviceGuy selected_drive;
  QString image_path;
};

#endif /* DISKWRITE_THREAD_H */
