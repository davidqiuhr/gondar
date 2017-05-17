
#include <QApplication>
#include <QLibraryInfo>

#include "gondarwizard.h"

extern "C" {
#include "neverware_unzipper.h"
}

#include <QtPlugin>

int main(int argc, char *argv[])
{
    /*
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
    Q_INIT_RESOURCE(gondarwizard);
    QApplication app(argc, argv);
    GondarWizard wizard;
    wizard.show();
    return app.exec();
    */
    neverware_unzip();
    return 0;
}
