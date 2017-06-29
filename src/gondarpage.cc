
#include "gondarpage.h"

#include <QLayout>

#include "gondarwizard.h"

GondarPage::GondarPage(QWidget* parent) : QWizardPage(parent) {
}

// Qt's documentation suggests next() or restart() should 'just work'
// however, using either of these seems to sometimes leave hanging displaying
// elements of the previous page
void GondarPage::sendError(QString errorStringIn) {
  if (layout()) {
    while (!layout()->isEmpty()) {
      QLayoutItem* curItem = layout()->takeAt(0);
      curItem->widget()->hide();
    }
  }
  GondarWizard* wiz = dynamic_cast<GondarWizard*>(wizard());
  wiz->goToErrorPage(errorStringIn);
}
