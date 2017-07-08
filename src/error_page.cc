#include "error_page.h"
#include <QDebug>

ErrorPage::ErrorPage() {
  setPixmap(QWizard::LogoPixmap, QPixmap(tr(":/images/crlogo.png")));
  setTitle(tr("An error has occurred"));
  setSubTitle(" ");

  error_.setWordWrap(true);

  layout_.addWidget(&error_);
  setLayout(&layout_);

  setFinalPage(true);
}

QString ErrorPage::error() const {
  return error_.text();
}

void ErrorPage::setError(const QString& error) {
  error_.setText(error);
}
