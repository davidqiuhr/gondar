// Copyright 2017 Neverware

#ifndef SRC_ERROR_PAGE_H_
#define SRC_ERROR_PAGE_H_

#include <QLabel>
#include <QVBoxLayout>

#include "wizard_page.h"

class ErrorPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  ErrorPage(QWidget* parent = 0);
  void setErrorString(const QString& errorString);
  bool errorEmpty() const;

 protected:
  int nextId() const override;
  void setVisible(bool visible) override;

 private:
  QVBoxLayout layout;
  QLabel label;
};

#endif  // SRC_ERROR_PAGE_H_
