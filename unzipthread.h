
#ifndef UNZIP_THREAD_H
#define UNZIP_THREAD_H

#include <QThread>
#include <QString>

class UnzipThread : public QThread {
    Q_OBJECT
  public:
    UnzipThread(QString * url_in, QObject *parent = 0);
    ~UnzipThread();

  signals:
    void complete();

  protected:
    void run() override;
    QString url;
};

#endif /* UNZIP_THREAD_H */
