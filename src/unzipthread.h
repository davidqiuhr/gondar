
#ifndef UNZIP_THREAD_H
#define UNZIP_THREAD_H

#include <QThread>
#include <QUrl>

class UnzipThread : public QThread {
  Q_OBJECT
 public:
  UnzipThread(const QUrl* url_in, QObject* parent = 0);
  ~UnzipThread();
  QString * getFilename();

 protected:
  void run() override;
  QUrl url;

 private:
  QString filename;
};

#endif /* UNZIP_THREAD_H */
