
#ifndef DISKWRITE_THREAD_H
#define DISKWRITE_THREAD_H

#include <QThread>
#include <QString>

#include "deviceguy.h"

class DiskWriteThread : public QThread {
    Q_OBJECT
  public:
    DiskWriteThread(DeviceGuy * drive_in,
                    const QString * image_path_in,
                    QObject *parent = 0);
    ~DiskWriteThread();

  signals:
    void usbcomplete();

  protected:
    void run() override;
  private:
    DeviceGuy selected_drive;
    QString image_path;
};

#endif /* DISKWRITE_THREAD_H */
