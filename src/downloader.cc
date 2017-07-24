#include "downloader.h"

#include <stdio.h>
#include <QDir>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "metric.h"

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent), downloadedCount(0), totalCount(0) {}

void DownloadManager::append(const QStringList& urlList) {
  foreach (QString url, urlList)
    append(QUrl::fromEncoded(url.toLocal8Bit()));

  if (downloadQueue.isEmpty())
    QTimer::singleShot(0, this, SIGNAL(finished()));
}

void DownloadManager::append(const QUrl& url) {
  if (downloadQueue.isEmpty())
    QTimer::singleShot(0, this, SLOT(startNextDownload()));

  downloadQueue.enqueue(url);
  ++totalCount;
}

QString DownloadManager::saveFileName(const QUrl& url) {
  return url.fileName();
}

void DownloadManager::startNextDownload() {
  if (downloadQueue.isEmpty()) {
    printf("%d/%d files downloaded successfully\n", downloadedCount,
           totalCount);
    emit finished();
    return;
  }

  QUrl url = downloadQueue.dequeue();

  const QDir dir =
      QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
  QString filename = saveFileName(url);

  output.setFileName(dir.filePath(filename));
  qInfo() << "Download destination:" << output.fileName();

  if (!output.open(QIODevice::WriteOnly)) {
    fprintf(stderr, "Problem opening save file '%s' for download '%s': %s\n",
            qPrintable(filename), url.toEncoded().constData(),
            qPrintable(output.errorString()));
    gondar::SendMetric("downloadAttempt");
    startNextDownload();
    return;  // skip this download
  }

  QNetworkRequest request(url);
  currentDownload = manager.get(request);
  connect(currentDownload, SIGNAL(finished()), SLOT(downloadFinished()));
  connect(currentDownload, SIGNAL(readyRead()), SLOT(downloadReadyRead()));
  emit started();

  // prepare the output
  printf("Downloading %s...\n", url.toEncoded().constData());
  downloadTime.start();
}

void DownloadManager::downloadFinished() {
  gondar::SendMetric("downloadSuccess");
  // progressBar.clear();
  output.close();

  if (currentDownload->error()) {
    // download failed
    fprintf(stderr, "Failed: %s\n", qPrintable(currentDownload->errorString()));
  } else {
    printf("Succeeded.\n");
    ++downloadedCount;
  }

  currentDownload->deleteLater();
  startNextDownload();
}

void DownloadManager::downloadReadyRead() {
  output.write(currentDownload->readAll());
}

QNetworkReply* DownloadManager::getCurrentDownload() {
  return currentDownload;
}

QFileInfo DownloadManager::outputFileInfo() const {
  return QFileInfo(output.fileName());
}
