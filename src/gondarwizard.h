
#ifndef GONDARWIZARD_H
#define GONDARWIZARD_H

#include <QProgressBar>
#include <QRadioButton>
#include <QString>
#include <QWizard>
#include <QtWidgets>

#include "admin_check_page.h"
#include "deviceguy.h"
#include "diskwritethread.h"
#include "downloader.h"
#include "image_select_page.h"
#include "unzipthread.h"

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

class DownloadProgressPage : public QWizardPage {
  Q_OBJECT

 public:
  DownloadProgressPage(QWidget* parent = 0);
  bool isComplete() const override;

 protected:
  void initializePage() override;
  void notifyUnzip();

 public slots:
  void markComplete();
  void downloadProgress(qint64 sofar, qint64 total);
  void onDownloadStarted();
  void onUnzipFinished();

 private:
  bool range_set;
  DownloadManager manager;
  QProgressBar progress;
  bool download_finished;
  QVBoxLayout layout;
  UnzipThread* unzipThread;
  QUrl url;
};

class UsbInsertPage : public QWizardPage {
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

class DeviceSelectPage : public QWizardPage {
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

class WriteOperationPage : public QWizardPage {
  Q_OBJECT

 public:
  WriteOperationPage(QWidget* parent = 0);

 protected:
  void initializePage() override;
  bool isComplete() const override;
  bool validatePage() override;
  void showProgress();
 public slots:
  void onDoneWriting();

 private:
  void writeToDrive();
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
  // There's an elaborate state-sharing solution via the 'field' mechanism
  // supported by QWizard.  I found the logic for that to be easy for sharing
  // some data types and convoluted for others.  In this case, a later page
  // makes a decision based on a radio button seleciton in an earlier page,
  // so putting the shared state in the wizard seems more straightforward
  AdminCheckPage adminCheckPage;
  ImageSelectPage imageSelectPage;
  DownloadProgressPage downloadProgressPage;
  UsbInsertPage usbInsertPage;
  DeviceSelectPage deviceSelectPage;
  WriteOperationPage writeOperationPage;

  void showUsualButtons();
  void showFinishButtons();
  // this enum determines page order
  enum {
    Page_adminCheck,
    Page_imageSelect,
    Page_usbInsert,
    Page_deviceSelect,
    Page_downloadProgress,
    Page_writeOperation
  };
 private slots:
  void handleMakeAnother();
};

#endif /* GONDARWIZARD */
