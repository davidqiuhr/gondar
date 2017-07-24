
#include "gondarwizard.h"

#include <QNetworkReply>
#include <QProgressBar>
#include <QtWidgets>

#include "diskwritethread.h"
#include "downloader.h"
#include "unzipthread.h"

#include "deviceguy.h"
#include "gondar.h"
#include "log.h"
#include "neverware_unzipper.h"

DeviceGuyList* drivelist = NULL;
DeviceGuy* selected_drive = NULL;

GondarWizard::GondarWizard(QWidget* parent) : QWizard(parent) {
  // these pages are automatically cleaned up
  // new instances are made whenever navigation moves on to another page
  // according to qt docs
  setPage(Page_adminCheck, &adminCheckPage);
  // chromeoverLogin and imageSelect are alternatives to each other
  // that both progress to usbInsertPage
  setPage(Page_chromeoverLogin, &chromeoverLoginPage);
  setPage(Page_siteSelect, &siteSelectPage);
  setPage(Page_imageSelect, &imageSelectPage);
  setPage(Page_usbInsert, &usbInsertPage);
  setPage(Page_deviceSelect, &deviceSelectPage);
  setPage(Page_downloadProgress, &downloadProgressPage);
  setPage(Page_writeOperation, &writeOperationPage);
  setPage(Page_error, &errorPage);
  setWizardStyle(QWizard::ModernStyle);
  setWindowTitle(tr("Cloudready USB Creation Utility"));
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));

  setOption(QWizard::HaveCustomButton1, false);
  setOption(QWizard::NoCancelButton, true);
  setOption(QWizard::NoBackButtonOnLastPage, true);
}

// handle event when 'make another usb' button pressed
void GondarWizard::handleMakeAnother() {
  // works as long as usbInsertPage is not the last page in wizard
  setStartId(usbInsertPage.nextId() - 1);
  restart();
}

int GondarWizard::nextId() const {
  if (errorPage.errorEmpty()) {
    return QWizard::nextId();
  } else {
    if (currentId() == Page_error) {
      return -1;
    } else {
      return Page_error;
    }
  }
}

void GondarWizard::postError(const QString& error) {
  QTimer::singleShot(0, this, [=]() { catchError(error); });
}

void GondarWizard::catchError(const QString& error) {
  LOG_ERROR << "displaying error: " << error;
  errorPage.setErrorString(error);
  next();
}

DownloadProgressPage::DownloadProgressPage(QWidget* parent)
    : WizardPage(parent) {
  setTitle("CloudReady Download");
  setSubTitle("Your installer is currently downloading.");
  download_finished = false;
  layout.addWidget(&progress);
  setLayout(&layout);
  range_set = false;
}

void DownloadProgressPage::initializePage() {
  // we don't support going back and redownloading right now
  // there's no real reason why we could not do this.  it just does not work
  // right now
  setCommitPage(true);
  setLayout(&layout);
  const QUrl url = wizard()->imageSelectPage.getUrl();
  qDebug() << "using url= " << url;
  connect(&manager, SIGNAL(finished()), this, SLOT(markComplete()));
  manager.append(url.toString());
  connect(&manager, SIGNAL(started()), this, SLOT(onDownloadStarted()));
}

void DownloadProgressPage::onDownloadStarted() {
  QNetworkReply* cur_download = manager.getCurrentDownload();
  connect(cur_download, &QNetworkReply::downloadProgress, this,
          &DownloadProgressPage::downloadProgress);
}

void DownloadProgressPage::downloadProgress(qint64 sofar, qint64 total) {
  if (!range_set) {
    range_set = true;
    progress.setRange(0, total);
  }
  progress.setValue(sofar);
}

void DownloadProgressPage::markComplete() {
  download_finished = true;
  // now that the download is finished, let's unzip the build.
  notifyUnzip();
  unzipThread = new UnzipThread(manager.outputFileInfo(), this);
  connect(unzipThread, SIGNAL(finished()), this, SLOT(onUnzipFinished()));
  unzipThread->start();
}

void DownloadProgressPage::onUnzipFinished() {
  // unzip has now completed
  qDebug() << "main thread has accepted complete";
  progress.setRange(0, 100);
  progress.setValue(100);
  setSubTitle("Download and extraction complete!");
  emit completeChanged();
  // immediately progress to writeOperationPage
  wizard()->next();
}
void DownloadProgressPage::notifyUnzip() {
  setSubTitle("Extracting compressed image...");
  // setting range and value to zero results in an 'infinite' progress bar
  progress.setRange(0, 0);
  progress.setValue(0);
}

bool DownloadProgressPage::isComplete() const {
  return download_finished;
}

const QString& DownloadProgressPage::getImageFileName() {
  return unzipThread->getFileName();
}

UsbInsertPage::UsbInsertPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Please insert an 8GB or 16GB USB storage device");
  setSubTitle(
      "In the next step, the selected device will be permanantly erased and "
      "turned into a CloudReady installer.");

  label.setText(
      "Sandisk devices are not recommended.  "
      "Devices with more than 16GB of space may be unreliable.  "
      "The next screen will become available once a valid "
      "destination drive is detected.");
  label.setWordWrap(true);

  layout.addWidget(&label);
  setLayout(&layout);

  // the next button should be grayed out until the user inserts a USB
  connect(this, SIGNAL(driveListRequested()), this, SLOT(getDriveList()));
}

void UsbInsertPage::initializePage() {
  tim = new QTimer(this);
  connect(tim, SIGNAL(timeout()), SLOT(getDriveList()));
  // if the page is visited again, delete the old drivelist
  if (drivelist != NULL) {
    DeviceGuyList_free(drivelist);
    drivelist = NULL;
  }
  // send a signal to check for drives
  emit driveListRequested();
}

bool UsbInsertPage::isComplete() const {
  // this should return false unless we have a non-empty result from
  // GetDevices()
  if (drivelist == NULL) {
    return false;
  } else {
    return true;
  }
}

void UsbInsertPage::getDriveList() {
  drivelist = GetDeviceList();
  if (DeviceGuyList_length(drivelist) == 0) {
    DeviceGuyList_free(drivelist);
    drivelist = NULL;
    tim->start(1000);
  } else {
    tim->stop();
    showDriveList();
  }
}

void UsbInsertPage::showDriveList() {
  emit completeChanged();
}

DeviceSelectPage::DeviceSelectPage(QWidget* parent) : WizardPage(parent) {
  // this page should just say 'hi how are you' while it stealthily loads
  // the usb device list.  or it could ask you to insert your device
  setTitle("USB device selection");
  setSubTitle("Choose your target device from the list of devices below.");

  label_.setText("Select Drive:");
  layout_.addWidget(&label_);
  layout_.addWidget(&device_picker_);
  setLayout(&layout_);
}

void DeviceSelectPage::initializePage() {
  device_picker_.refresh();
}

bool DeviceSelectPage::validatePage() {
  return device_picker_.hasSelection();
}

int DeviceSelectPage::nextId() const {
  if (wizard()->downloadProgressPage.isComplete()) {
    return GondarWizard::Page_writeOperation;
  } else {
    return GondarWizard::Page_downloadProgress;
  }
}

WriteOperationPage::WriteOperationPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Creating your CloudReady USB installer");
  setSubTitle("This process may take up to 20 minutes.");
  layout.addWidget(&progress);
  setLayout(&layout);
}

void WriteOperationPage::initializePage() {
  writeFinished = false;
  // what if we just start writing as soon as we get here
  if (selected_drive == NULL) {
    qDebug() << "ERROR: no drive selected";
  } else {
    writeToDrive();
  }
}

bool WriteOperationPage::isComplete() const {
  return writeFinished;
}

bool WriteOperationPage::validatePage() {
  return writeFinished;
}

void WriteOperationPage::writeToDrive() {
  qDebug() << "Writing to drive...";
  image_path.clear();
  image_path.append(wizard()->downloadProgressPage.getImageFileName());
  showProgress();
  diskWriteThread = new DiskWriteThread(selected_drive, image_path, this);
  connect(diskWriteThread, SIGNAL(finished()), this, SLOT(onDoneWriting()));
  qDebug() << "launching thread...";
  diskWriteThread->start();
}

void WriteOperationPage::showProgress() {
  progress.setRange(0, 0);
  progress.setValue(0);
}

void WriteOperationPage::onDoneWriting() {
  switch (diskWriteThread->state()) {
    case DiskWriteThread::State::Initial:
    case DiskWriteThread::State::Running:
      // It should not be possible to get here at runtime
      writeFailed("Internal state error");
      return;

    case DiskWriteThread::State::GetFileSizeFailed:
      writeFailed("Error reading the disk image's file size");
      return;

    case DiskWriteThread::State::InstallFailed:
      writeFailed("Error writing to the USB device");
      return;

    case DiskWriteThread::State::Success:
      // Hooray!
      break;
  }

  setTitle("CloudReady USB created!");
  setSubTitle("You may now either exit or create another USB.");
  qDebug() << "install call returned";
  writeFinished = true;
  progress.setRange(0, 100);
  progress.setValue(100);
  wizard()->setOption(QWizard::HaveCustomButton1, true);
  emit completeChanged();
}

// though error page follows in index, this is the end of the wizard for
// healthy flows
int WriteOperationPage::nextId() const {
  return -1;
}

void WriteOperationPage::setVisible(bool visible) {
  WizardPage::setVisible(visible);
  GondarWizard* wiz = dynamic_cast<GondarWizard*>(wizard());
  if (visible) {
    setButtonText(QWizard::CustomButton1, "Make Another USB");
    connect(wiz, SIGNAL(customButtonClicked(int)), wiz,
            SLOT(handleMakeAnother()));
  } else {
    wiz->setOption(QWizard::HaveCustomButton1, false);
    disconnect(wiz, SIGNAL(customButtonClicked(int)), wiz,
               SLOT(handleMakeAnother()));
  }
}

void WriteOperationPage::writeFailed(const QString& errorMessage) {
  wizard()->postError(errorMessage);
  writeFinished = true;
  emit completeChanged();
}

ErrorPage::ErrorPage(QWidget* parent) : WizardPage(parent) {
  setTitle("An error has occurred");
  setSubTitle(" ");
  layout.addWidget(&label);
  label.setText("");
  setLayout(&layout);
}

void ErrorPage::setErrorString(const QString& errorStringIn) {
  label.setText(errorStringIn);
}

bool ErrorPage::errorEmpty() const {
  return label.text().isEmpty();
}

int ErrorPage::nextId() const {
  return -1;
}

void ErrorPage::setVisible(bool visible) {
  WizardPage::setVisible(visible);
  if (visible) {
    setButtonText(QWizard::CustomButton1, "Exit");
    wizard()->setOption(QWizard::HaveCustomButton1, true);
    connect(wizard(), SIGNAL(customButtonClicked(int)),
            QApplication::instance(), SLOT(quit()));
  } else {
    wizard()->setOption(QWizard::HaveCustomButton1, false);
    disconnect(wizard(), SIGNAL(customButtonClicked(int)),
               QApplication::instance(), SLOT(quit()));
  }
}
