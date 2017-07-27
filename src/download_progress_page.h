// Copyright 2017 Neverware

#ifndef SRC_DOWNLOAD_PROGRESS_PAGE_H_
#define SRC_DOWNLOAD_PROGRESS_PAGE_H_

#include <QProgressBar>
#include <QVBoxLayout>

#include "downloader.h"
#include "unzipthread.h"
#include "wizard_page.h"

class DownloadProgressPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  DownloadProgressPage(QWidget* parent = 0);
  bool isComplete() const override;
  const QString& getImageFileName();

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
};

#endif  // SRC_DOWNLOAD_PROGRESS_PAGE_H_
