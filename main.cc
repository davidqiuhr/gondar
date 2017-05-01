
#include <QApplication>
#include <QLibraryInfo>

#include "gondarwizard.h"

// temp debug
extern "C" {
#include "gondar.h"
#include "deviceguy.h"
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gondarwizard);
    QApplication app(argc, argv);
    GondarWizard wizard;
    wizard.show();
    return app.exec();
    /*
    DeviceGuyList * devlist = GetDeviceList();
    DeviceGuyList_print(devlist);
    char * image_path = "c:\\cloudready.bin";
    Install(devlist->head, image_path);
    */
    return 0;
}
