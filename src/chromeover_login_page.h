#ifndef CHROMEOVER_LOGIN_PAGE_H
#define CHROMEOVER_LOGIN_PAGE_H

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

class ChromeoverLoginPage : public QWizardPage {
  Q_OBJECT

 public:
  ChromeoverLoginPage(QWidget* parent = 0);
  int nextId() const override;

 private:
  QGridLayout layout;
  QLineEdit usernameLineEdit;
  QLabel usernameLineEditLabel;
  QLineEdit passwordLineEdit;
  QLabel passwordLineEditLabel;
};

#endif
