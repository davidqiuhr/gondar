
#include "diskwritethread.h"

#include "string.h"

#include "deviceguy.h"

#include <QtWidgets>

extern "C" {
#include "gondar.h"
}

DiskWriteThread::DiskWriteThread(QObject *parent)
    : QThread(parent) {
    memset(image_path, 0, MAX_STRING);
}

DiskWriteThread::~DiskWriteThread() {
}

void DiskWriteThread::setDrive(DeviceGuy * drive_in) {
    selected_drive = drive_in;
}

void DiskWriteThread::setImagePath(const char * image_path_in) {
    strcpy(image_path, image_path_in);
}

void DiskWriteThread::launchThread() {
    if (!isRunning()) {
        start(LowPriority);
    }
}

void DiskWriteThread::run() {
    qDebug() << "running diskwrite on image=" << image_path;
    Install(selected_drive, image_path);
    qDebug() << "worker thread says complete";
    emit usbcomplete();
}
