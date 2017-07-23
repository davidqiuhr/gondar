// Copyright 2017 Neverware

#ifndef SRC_WIZARD_PAGE_H_
#define SRC_WIZARD_PAGE_H_

#include <QVBoxLayout>
#include <QWizard>

#include "wizard_header.h"

class GondarWizard;

namespace gondar {

// For now this class is almost the same as a QWizardPage, but it'll
// be a helpful stepping stone for replacing QWizard with an
// implementation better tailored to our needs.
//
// The one minor piece of functionality it provides is that wizard()
// returns GondarWizard directly, rather than a generic QWizard
// pointer.
class WizardPage : public QWizardPage {
  Q_OBJECT

 public:
  explicit WizardPage(QWidget* parent = nullptr);

 protected:
  GondarWizard* wizard() const;

  void setLayout(QLayout* layout);

  void initializePage() override;

 private:
  QVBoxLayout layout_;
  WizardHeader header_;
  QWidget center_;
};
}

#endif  // SRC_WIZARD_PAGE_H_
