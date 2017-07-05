
#ifndef UNZIP_THREAD_H
#define UNZIP_THREAD_H

#include <QFileInfo>
#include <QThread>

class UnzipThread : public QThread {
  Q_OBJECT
 public:
  UnzipThread(const QFileInfo& inputFile, QObject* parent = 0);
  ~UnzipThread();
  const QString& getFileName() const;

 protected:
  void run() override;

 private:
  QFileInfo inputFile;
  QString filename;
};

#endif /* UNZIP_THREAD_H */
