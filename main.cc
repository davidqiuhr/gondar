
#include <QApplication>
#include <QLibraryInfo>

//temp
#include <QUrl>

#include "gondarwizard.h"

//temp
#include "downloader.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(gondarwizard);

    QApplication app(argc, argv);

    GondarWizard wizard;
    wizard.show();
    return app.exec();
}
