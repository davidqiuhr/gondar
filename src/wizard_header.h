// Copyright 2017 Neverware

#ifndef SRC_WIZARD_HEADER_H_
#define SRC_WIZARD_HEADER_H_

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "util.h"

namespace gondar {

class WizardHeader : public QWidget {
  Q_OBJECT

 public:
  WizardHeader();

  void setTitle(const QString& title);
  void setSubtitle(const QString& subtitle);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QVBoxLayout layout_;
  QHBoxLayout title_and_logo_layout_;
  QVBoxLayout title_layout_;
  QLabel title_label_;
  QLabel subtitle_label_;
  QLabel logo_label_;
  HorizontalLine divider_;
};

}  // namespace gondar

#endif  // SRC_WIZARD_HEADER_H_
