
#include <QApplication>
#include <QLibraryInfo>

#include "gondarwizard.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gondarwizard);
    QApplication app(argc, argv);
    GondarWizard wizard;
    wizard.show();
    return app.exec();
}
