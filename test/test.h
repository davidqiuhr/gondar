// Copyright 2017 Neverware

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <QObject>

#include <QtTest/QtTest>

namespace gondar {

class Test : public QObject {
  Q_OBJECT

 private slots:
  void sanity();
};

}

#endif  // TEST_TEST_H_
