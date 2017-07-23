
#include <QApplication>
#include <QFile>
#include <QLibraryInfo>
#include <QtPlugin>

#include "gondarwizard.h"
#include "log.h"

namespace {

void loadStyleSheet(QApplication* app) {
  QFile file(":/style.css");
  file.open(QFile::ReadOnly);
  const QString content = file.readAll();
  app->setStyleSheet(content);
}

}  // namespace

int main(int argc, char* argv[]) {
#if defined(Q_OS_WIN)
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif
  Q_INIT_RESOURCE(gondarwizard);

  gondar::InitializeLogging();

  QApplication app(argc, argv);
  loadStyleSheet(&app);

  GondarWizard wizard;
  wizard.show();

  const auto ret = app.exec();
  LOG_INFO << "app.exec() returned " << ret;

  return ret;
}
