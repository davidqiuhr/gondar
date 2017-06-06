
#include "diskwritethread.h"

#include "deviceguy.h"

#include <QtWidgets>

extern "C" {
#include "gondar.h"
}

DiskWriteThread::DiskWriteThread(QObject *parent)
    : QThread(parent) {
}

DiskWriteThread::~DiskWriteThread() {
}

void DiskWriteThread::setDrive(DeviceGuy * drive_in) {
    DeviceGuy_copy(drive_in, & selected_drive);
}

void DiskWriteThread::setImagePath(QString * image_path_in) {
    image_path.clear();
    image_path.append(image_path_in);
}

void DiskWriteThread::launchThread() {
    if (!isRunning()) {
        start();
    }
}

void DiskWriteThread::run() {
    qDebug() << "running diskwrite on image=" << image_path;
    const char * image_path_c_str = image_path.toStdString().c_str();
    Install(& selected_drive, image_path_c_str);
    qDebug() << "worker thread says complete";
    emit usbcomplete();
}
