
#ifndef DISKWRITE_THREAD_H
#define DISKWRITE_THREAD_H

#include <QThread>

#include "deviceguy.h"

#define MAX_STRING 300

class DiskWriteThread : public QThread {
    Q_OBJECT
  public:
    DiskWriteThread(QObject *parent = 0);
    void setDrive(DeviceGuy * drive_in);
    void setImagePath(const char * image_path_in);
    ~DiskWriteThread();
    void launchThread();

  signals:
    void usbcomplete();

  protected:
    void run() override;
    DeviceGuy *  selected_drive;
    char image_path[MAX_STRING];
};

#endif /* DISKWRITE_THREAD_H */
