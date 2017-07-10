#ifndef IMAGE_SELECT_PAGE_H
#define IMAGE_SELECT_PAGE_H

#include <QButtonGroup>
#include <QRadioButton>
#include <QUrl>
#include <QVBoxLayout>

#include "wizard_page.h"

class ImageSelectPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  ImageSelectPage(QWidget* parent = 0);
  QUrl getUrl() const;

 private:
  QButtonGroup bitnessButtons;
  QRadioButton thirtyTwo;
  QRadioButton sixtyFour;
  QVBoxLayout layout;
  QUrl thirtyTwoUrl;
  QUrl sixtyFourUrl;
};

#endif
