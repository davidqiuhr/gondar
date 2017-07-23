// Copyright 2017 Neverware

#include "widget_utils.h"

namespace gondar {

HorizontalLine::HorizontalLine() {
  setFrameShape(QFrame::HLine);
  setFrameShadow(QFrame::Sunken);
  setMaximumHeight(2);
}

void setEmptyMargins(QLayout* layout) {
  layout->setContentsMargins(0, 0, 0, 0);
}

void setDefaultMargins(QLayout* layout) {
  const int margin = 12;
  layout->setContentsMargins(margin, margin, margin, margin);
}

}  // namespace gondar
