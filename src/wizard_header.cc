// Copyright 2017 Neverware

#include "wizard_header.h"

namespace gondar {

WizardHeader::WizardHeader() {
  subtitle_label_.setWordWrap(true);
  logo_label_.setPixmap(QPixmap(":/images/crlogo.png"));
  logo_label_.setAlignment(Qt::AlignCenter);
  title_label_.setObjectName("title");
  setObjectName("wizardHeader");

  title_layout_.addWidget(&title_label_);
  title_layout_.addWidget(&subtitle_label_, 1);
  title_layout_.setSpacing(10);

  title_and_logo_layout_.addLayout(&title_layout_, 1);
  title_and_logo_layout_.addWidget(&logo_label_);
  title_and_logo_layout_.setSpacing(24);
  setDefaultMargins(&title_and_logo_layout_);

  layout_.addLayout(&title_and_logo_layout_);
  layout_.addWidget(&divider_);
  layout_.setSpacing(0);
  setEmptyMargins(&layout_);
  setLayout(&layout_);
}

void WizardHeader::setTitle(const QString& title) {
  title_label_.setText(title);
}

void WizardHeader::setSubtitle(const QString& subtitle) {
  subtitle_label_.setText(subtitle);
}

}  // namespace gondar
