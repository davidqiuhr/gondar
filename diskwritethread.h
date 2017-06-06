
#ifndef DISKWRITE_THREAD_H
#define DISKWRITE_THREAD_H

#include <QThread>
#include <QString>

#include "deviceguy.h"

class DiskWriteThread : public QThread {
    Q_OBJECT
  public:
    DiskWriteThread(QObject *parent = 0);
    void setDrive(DeviceGuy * drive_in);
    void setImagePath(QString * image_path_in);
    ~DiskWriteThread();
    void launchThread();

  signals:
    void usbcomplete();

  protected:
    void run() override;
    DeviceGuy *  selected_drive;
    QString image_path;
};

#endif /* DISKWRITE_THREAD_H */
