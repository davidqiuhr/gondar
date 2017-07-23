// Copyright 2017 Neverware

#ifndef SRC_WIDGET_UTILS_H_
#define SRC_WIDGET_UTILS_H_

#include <QFrame>
#include <QLayout>

namespace gondar {

class HorizontalLine : public QFrame {
 public:
  HorizontalLine();
};

void setEmptyMargins(QLayout* layout);

void setDefaultMargins(QLayout* layout);

}  // namespace gondar

#endif  // SRC_WIDGET_UTILS_H_
