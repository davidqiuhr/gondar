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

#include <QtTest/QtTest>

#include "src/device_picker.h"

#include "src/gondarwizard.h"
#include "src/util.h"

namespace gondar {

class Test : public QObject {
  Q_OBJECT

 private slots:
  void testDevicePicker();
  void testLinuxStubFlow();
};

class TestWizard::Private {
 public:  
  gondar::UpdateCheck updateCheck;
  gondar::AboutDialog aboutDialog;
  
  AdminCheckPage adminCheckPage;
  // use a special test version of device select page
  TestDeviceSelectPage deviceSelectPage;
  ChromeoverLoginPage chromeoverLoginPage;
  SiteSelectPage siteSelectPage;
  ErrorPage errorPage;
  
  std::vector<GondarSite> sites;

  QDateTime runTime;
};

// test objects
class TestDevicePicker : public DevicePicker {
 public:
  TestDevicePicker();
 private:
  const DevicePicker::Button* selectedButton() const;
};

class TestDeviceSelectPage : public DeviceSelectPage {
 protected:
  void initializePage() override; 
  // TODO: what should the return of this be?
  void getPrivate() override;
  class Private;
  std::unique_ptr<Private> p_;
};

class TestWizard : public GondarWizard {
  Q_OBJECT
 public:
  TestWizard(QWidget* parent = 0);
};
}

#endif  // TEST_TEST_H_
