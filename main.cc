
#include <QApplication>
#include <QLibraryInfo>

#include "gondarwizard.h"

//temp debug
/*
extern "C" {
#include "gondar.h"
#include "deviceguy.h"
}
*/

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gondarwizard);
    QApplication app(argc, argv);
    GondarWizard wizard;
    wizard.show();
    return app.exec();
    //printf("is elevated? %d\n", IsCurrentProcessElevated());
}
