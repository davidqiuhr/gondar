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

#ifndef SRC_USB_INSERT_PAGE_H_
#define SRC_USB_INSERT_PAGE_H_

#include <QLabel>
#include <QVBoxLayout>

#include "device.h"
#include "wizard_page.h"

class UsbInsertPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  explicit UsbInsertPage(QWidget* parent = 0);

  const DeviceGuyList& devices() const;

 protected:
  void initializePage() override;
  bool isComplete() const override;

 private:
  void showDriveList();
  DeviceGuyList drivelist;
  QLabel label;
  QVBoxLayout layout;

 public slots:
  void getDriveList();
 signals:
  void driveListRequested();
};

#endif  // SRC_USB_INSERT_PAGE_H_
