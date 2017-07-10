#ifndef ADMIN_CHECK_PAGE_H
#define ADMIN_CHECK_PAGE_H

#include <QLabel>
#include <QVBoxLayout>

#include "wizard_page.h"

class AdminCheckPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  AdminCheckPage(QWidget* parent = 0);
  int nextId() const override;

 protected:
  void initializePage() override;
  bool isComplete() const override;
  void showIsAdmin();
  void showIsNotAdmin();

 private:
  static bool isChromeover();
  QLabel label;
  bool is_admin;
  QVBoxLayout layout;
};

#endif
