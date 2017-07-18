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
  int nextId() const override;
  void set32Url(QUrl url_in);
  void set64Url(QUrl url_in);

 private:
  QButtonGroup bitnessButtons;
  QRadioButton thirtyTwo;
  QRadioButton sixtyFour;
  QVBoxLayout layout;
  QUrl thirtyTwoUrl;
  QUrl sixtyFourUrl;
};

#endif
