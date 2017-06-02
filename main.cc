
#include <QApplication>
#include <QLibraryInfo>

#include "gondarwizard.h"

#include <QtPlugin>

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif
    Q_INIT_RESOURCE(gondarwizard);
    QApplication app(argc, argv);
    GondarWizard wizard;
    wizard.show();
    return app.exec();
}
