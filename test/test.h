// Copyright 2017 Neverware
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <QObject>
#include <QWidget>

#include <QtTest/QtTest>

// for test objects
#include "src/device_picker.h"
#include "src/diskwritethread.h"
#include "src/download_progress_page.h"
#include "src/unzipthread.h"
#include "src/write_operation_page.h"

// test objects
class TestDevicePicker : public gondar::DevicePicker {
 public:
  TestDevicePicker();
 private:
  const DevicePicker::Button* selectedButton() const override;
};

class TestUnzipThread : public UnzipThread {
  Q_OBJECT
 public:
  TestUnzipThread(const QFileInfo& inputFile, QObject* parent = 0);
  const QString& getFileName() const override;
  const QString& kewlstr = ""; 
  void run() override;
};

class TestDownloadProgressPage : public DownloadProgressPage {
  Q_OBJECT
 public:
  TestDownloadProgressPage(QWidget* parent = 0);
  UnzipThread * makeUnzipThread() override; 
};

class TestDiskWriteThread : public DiskWriteThread {
  Q_OBJECT
 public:
  TestDiskWriteThread(DeviceGuy * drive_in, const QString& image_path_in, QObject* parent = 0);
  void run() override;
};

class TestWriteOperationPage : public WriteOperationPage {
 public:
  TestWriteOperationPage(QWidget* parent = 0);
  DiskWriteThread * makeDiskWriteThread(DeviceGuy* drive_in, const QString& image_path_in, QObject* parent) override; 
};

namespace gondar {

class Test : public QObject {
  Q_OBJECT

 private slots:
  void testDevicePicker();
  void testLinuxStubFlow();
};
}

#endif  // TEST_TEST_H_
