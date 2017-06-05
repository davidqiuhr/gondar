
#ifndef UNZIP_THREAD_H
#define UNZIP_THREAD_H

#include <QThread>

#define MAX_URL 300

class UnzipThread : public QThread {
    Q_OBJECT
  public:
    UnzipThread(QObject *parent = 0);
    void setUrl(const char * url_in);
    ~UnzipThread();
    void launchThread();

  signals:
    void complete();

  protected:
    void run() override;
    char url[MAX_URL];
};

#endif /* UNZIP_THREAD_H */
