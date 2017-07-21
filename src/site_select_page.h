
#ifndef SITE_SELECT_PAGE_H
#define SITE_SELECT_PAGE_H

#include <QButtonGroup>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include "gondarsite.h"
#include "wizard_page.h"

class SiteSelectPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  SiteSelectPage(QWidget* parent = 0);

 protected:
  void initializePage() override;
  bool validatePage() override;

 private:
  QButtonGroup sitesButtons;
  QVBoxLayout layout;
};

#endif
