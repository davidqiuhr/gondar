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

#ifndef GONDARWIZARD_H
#define GONDARWIZARD_H

#include <QProgressBar>
#include <QRadioButton>
#include <QShortcut>
#include <QString>
#include <QWizard>
#include <QtWidgets>

#include "about_dialog.h"
#include "admin_check_page.h"
#include "chromeover_login_page.h"
#include "device.h"
#include "diskwritethread.h"
#include "download_progress_page.h"
#include "error_page.h"
#include "image_select_page.h"
#include "site_select_page.h"
#include "wizard_page.h"

class QCheckBox;
class QGroupBox;
class QLabel;
class QRadioButton;

class GondarButton : public QRadioButton {
  Q_OBJECT

 public:
  GondarButton(const QString& text,
               unsigned int device_num,
               QWidget* parent = 0);
  unsigned int index = 0;
};

class UsbInsertPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  UsbInsertPage(QWidget* parent = 0);

 protected:
  void initializePage() override;
  bool isComplete() const override;

 private:
  void showDriveList();
  QLabel label;
  QTimer* tim;
  QVBoxLayout layout;

 public slots:
  void getDriveList();
 signals:
  void driveListRequested();
};

class DeviceSelectPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  DeviceSelectPage(QWidget* parent = 0);
  int nextId() const override;

 protected:
  void initializePage() override;
  bool validatePage() override;

 private:
  QLabel drivesLabel;
  QGroupBox* drivesBox;
  QButtonGroup* radioGroup;
  QVBoxLayout* layout;
};

class WriteOperationPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  WriteOperationPage(QWidget* parent = 0);

 protected:
  void initializePage() override;
  bool isComplete() const override;
  bool validatePage() override;
  void showProgress();
  int nextId() const override;
  void setVisible(bool visible) override;
 public slots:
  void onDoneWriting();

 private:
  void writeToDrive();
  void writeFailed(const QString& errorMessage);
  QVBoxLayout layout;
  QProgressBar progress;
  bool writeFinished;
  DiskWriteThread* diskWriteThread;
  QString image_path;
};

class GondarWizard : public QWizard {
  Q_OBJECT

 public:
  GondarWizard(QWidget* parent = 0);

  int nextId() const override;
  void postError(const QString& error);
  double getRunTime();
  // There's an elaborate state-sharing solution via the 'field' mechanism
  // supported by QWizard.  I found the logic for that to be easy for sharing
  // some data types and convoluted for others.  In this case, a later page
  // makes a decision based on a radio button seleciton in an earlier page,
  // so putting the shared state in the wizard seems more straightforward
  AdminCheckPage adminCheckPage;
  ErrorPage errorPage;
  ChromeoverLoginPage chromeoverLoginPage;
  SiteSelectPage siteSelectPage;
  ImageSelectPage imageSelectPage;
  DownloadProgressPage downloadProgressPage;
  UsbInsertPage usbInsertPage;
  DeviceSelectPage deviceSelectPage;
  WriteOperationPage writeOperationPage;

  // this enum determines page order
  enum {
    Page_adminCheck,
    Page_chromeoverLogin,
    Page_siteSelect,
    Page_imageSelect,
    Page_usbInsert,
    Page_deviceSelect,
    Page_downloadProgress,
    Page_writeOperation,
    Page_error
  };

 private slots:
  void handleMakeAnother();

 private:
  void catchError(const QString& error);

  QShortcut about_shortcut_;
  QTime runTime;
  gondar::AboutDialog about_dialog_;
};

#endif /* GONDARWIZARD */
