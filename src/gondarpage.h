
#ifndef GONDARPAGE_H
#define GONDARPAGE_H

#include <QWizardPage>

class GondarPage : public QWizardPage {
  Q_OBJECT

 public:
  GondarPage(QWidget* parent = 0);
  void sendError(QString errorStringIn);
};

#endif /* GONDARPAGE_H */
