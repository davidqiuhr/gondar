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
  explicit DownloadProgressPage(QWidget* parent = 0);
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
