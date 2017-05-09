
#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QNetworkAccessManager>
#include <QTimer>
#include <QUrl>

class DownloadManager: public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
    QList<QNetworkReply *> currentDownloads;

public:
    DownloadManager();
    QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    void doDownload(const QUrl &url);
    QUrl url;

public slots:
    void execute();
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);

public signals:
    void finished(); 
};

#endif /* DOWNLOADER_H */
