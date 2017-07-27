// Copyright 2017 Neverware

#include "download_progress_page.h"
#include "gondarwizard.h"

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
