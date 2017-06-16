
#ifndef GONDARWIZARD_H
#define GONDARWIZARD_H

#include <QWizard>
#include <QRadioButton>
#include <QProgressBar>
#include <QtWidgets>
#include <QString>

#include "diskwritethread.h"
#include "unzipthread.h"
#include "deviceguy.h"
#include "downloader.h"

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

class AdminCheckPage : public QWizardPage {
  Q_OBJECT

 public:
  AdminCheckPage(QWidget* parent = 0);

 protected:
  void initializePage() override;
  bool isComplete() const;
  void showIsAdmin();
  void showIsNotAdmin();

 private:
  QLabel label;
  bool is_admin;
  QVBoxLayout layout;
};

class ImageSelectPage : public QWizardPage {
  Q_OBJECT

 public:
  ImageSelectPage(QWidget* parent = 0);
  QUrl getUrl();

 private:
  QButtonGroup bitnessButtons;
  QRadioButton thirtyTwo;
  QRadioButton sixtyFour;
  QVBoxLayout layout;
  QUrl thirtyTwoUrl;
  QUrl sixtyFourUrl;
};

class DownloadProgressPage : public QWizardPage {
  Q_OBJECT

 public:
  DownloadProgressPage(QWidget* parent = 0);

 protected:
  void initializePage() override;
  bool isComplete() const;
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
  bool isComplete() const;

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
  int nextId() const;

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
  bool isComplete() const;
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

  QRadioButton* bitnessSelected;
  void showUsualButtons();
  void showFinishButtons();
  bool isDownloaded;
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
