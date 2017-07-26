// Copyright 2017 Neverware

#ifndef SRC_ABOUT_DIALOG_H_
#define SRC_ABOUT_DIALOG_H_

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace gondar {

class AboutDialog : public QDialog {
  Q_OBJECT

 public:
  AboutDialog();

 private:
  QVBoxLayout layout_;
  QLabel about_label_;
  QTextBrowser license_text_browser_;
  QPushButton close_button_;
};
}

#endif  // SRC_ABOUT_DIALOG_H_
