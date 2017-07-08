
#include <QNetworkReply>
#include <QProgressBar>
#include <QtWidgets>

#include "diskwritethread.h"
#include "downloader.h"
#include "gondarwizard.h"
#include "unzipthread.h"

#include "deviceguy.h"
#include "gondar.h"
#include "neverware_unzipper.h"

DeviceGuyList* drivelist = NULL;
DeviceGuy* selected_drive = NULL;

GondarButton::GondarButton(const QString& text,
                           unsigned int device_num,
                           QWidget* parent)
    : QRadioButton(text, parent) {
  index = device_num;
}
GondarWizard::GondarWizard(QWidget* parent) : QWizard(parent) {
  // these pages are automatically cleaned up
  // new instances are made whenever navigation moves on to another page
  // according to qt docs
  setPage(Page_adminCheck, &adminCheckPage);
  setPage(Page_imageSelect, &imageSelectPage);
  setPage(Page_usbInsert, &usbInsertPage);
  setPage(Page_deviceSelect, &deviceSelectPage);
  setPage(Page_downloadProgress, &downloadProgressPage);
  setPage(Page_writeOperation, &writeOperationPage);
  setWizardStyle(QWizard::ModernStyle);
  setWindowTitle(tr("Cloudready USB Creation Utility"));

  // Only show next buttons for all screens
  QList<QWizard::WizardButton> button_layout;
  button_layout << QWizard::NextButton;
  setButtonLayout(button_layout);

  setButtonText(QWizard::CustomButton1, tr("Make Another USB"));
  connect(this, SIGNAL(customButtonClicked(int)), this,
          SLOT(handleMakeAnother()));
}

// handle event when 'make another usb' button pressed
void GondarWizard::handleMakeAnother() {
  // we set the page to usbInsertPage and show usual buttons
  showUsualButtons();
  // works as long as usbInsertPage is not the last page in wizard
  setStartId(usbInsertPage.nextId() - 1);
  restart();
}
void GondarWizard::showUsualButtons() {
  // Only show next buttons for all screens
  QList<QWizard::WizardButton> button_layout;
  button_layout << QWizard::NextButton;
  setOption(QWizard::HaveCustomButton1, false);
  setButtonLayout(button_layout);
}

// show 'make another usb' button along with finish button at end of wizard
void GondarWizard::showFinishButtons() {
  QList<QWizard::WizardButton> button_layout;
  button_layout << QWizard::FinishButton;
  button_layout << QWizard::CustomButton1;
  setOption(QWizard::HaveCustomButton1, true);
  setButtonLayout(button_layout);
}

DownloadProgressPage::DownloadProgressPage(QWidget* parent)
    : WizardPage(parent) {
  setTitle("CloudReady Download");
  setSubTitle("Your installer is currently downloading.");
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));
  download_finished = false;
  layout.addWidget(&progress);
  setLayout(&layout);
  range_set = false;
}

void DownloadProgressPage::initializePage() {
  setLayout(&layout);
  GondarWizard* wiz = dynamic_cast<GondarWizard*>(wizard());
  const QUrl url = wiz->imageSelectPage.getUrl();
  qDebug() << "using url= " << url;
  QObject::connect(&manager, SIGNAL(finished()), this, SLOT(markComplete()));
  manager.append(url.toString());
  QObject::connect(&manager, SIGNAL(started()), this,
                   SLOT(onDownloadStarted()));
}

void DownloadProgressPage::onDownloadStarted() {
  QNetworkReply* cur_download = manager.getCurrentDownload();
  QObject::connect(cur_download, &QNetworkReply::downloadProgress, this,
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
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));

  label.setText(
      "Sandisk devices are not recommended.  "
      "Devices with more than 16GB of space may be unreliable.  "
      "The next screen will become available once a valid "
      "destination drive is detected.");
  label.setWordWrap(true);

  layout.addWidget(&label);
  setLayout(&layout);

  // the next button should be grayed out until the user inserts a USB
  QObject::connect(this, SIGNAL(driveListRequested()), this,
                   SLOT(getDriveList()));
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
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));
  layout = new QVBoxLayout;
  drivesLabel.setText("Select Drive:");
  radioGroup = NULL;
  setLayout(layout);
}

void DeviceSelectPage::initializePage() {
  // while our layout is not empty, remove items from it
  while (!layout->isEmpty()) {
    QLayoutItem* curItem = layout->takeAt(0);
    if (curItem->widget() != &drivesLabel) {
      delete curItem->widget();
    }
  }

  // remove our last listing
  delete radioGroup;

  if (drivelist == NULL) {
    return;
  }
  DeviceGuy* itr = drivelist->head;
  // Line up widgets horizontally
  // use QVBoxLayout for vertically, H for horizontal
  layout->addWidget(&drivesLabel);

  radioGroup = new QButtonGroup();
  // i could extend the button object to also have a secret index
  // then i could look up index later easily
  while (itr != NULL) {
    // FIXME(kendall): clean these up
    GondarButton* curRadio = new GondarButton(itr->name, itr->device_num, this);
    radioGroup->addButton(curRadio);
    layout->addWidget(curRadio);
    itr = itr->next;
  }
  setLayout(layout);
}

bool DeviceSelectPage::validatePage() {
  // TODO(kendall): check for NULL on bad cast
  GondarButton* selected =
      dynamic_cast<GondarButton*>(radioGroup->checkedButton());
  if (selected == NULL) {
    return false;
  } else {
    unsigned int selected_index = selected->index;
    selected_drive = DeviceGuyList_getByIndex(drivelist, selected_index);
    return true;
  }
}

int DeviceSelectPage::nextId() const {
  GondarWizard* wiz = dynamic_cast<GondarWizard*>(wizard());
  if (wiz->downloadProgressPage.isComplete()) {
    return GondarWizard::Page_writeOperation;
  } else {
    return GondarWizard::Page_downloadProgress;
  }
}

WriteOperationPage::WriteOperationPage(QWidget* parent) : WizardPage(parent) {
  setTitle("Creating your CloudReady USB installer");
  setSubTitle("This process may take up to 20 minutes.");
  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/crlogo.png"));
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
  GondarWizard* wiz = dynamic_cast<GondarWizard*>(wizard());
  image_path.append(wiz->downloadProgressPage.getImageFileName());
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
  setTitle("CloudReady USB created!");
  setSubTitle("You may now either exit or create another USB.");
  qDebug() << "install call returned";
  writeFinished = true;
  progress.setRange(0, 100);
  progress.setValue(100);
  GondarWizard* wiz = dynamic_cast<GondarWizard*>(wizard());
  wiz->showFinishButtons();
  emit completeChanged();
}
