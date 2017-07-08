// Copyright 2017 Neverware

#ifndef SRC_ERROR_PAGE_H_
#define SRC_ERROR_PAGE_H_

#include <QLabel>
#include <QVBoxLayout>
#include <QWizardPage>

class ErrorPage : public QWizardPage {
 public:
  ErrorPage();

  QString error() const;
  void setError(const QString& error);

 private:
  QVBoxLayout layout_;
  QLabel error_;
};

#endif  // SRC_ERROR_PAGE_H_
