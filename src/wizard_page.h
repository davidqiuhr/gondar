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

#ifndef SRC_WIZARD_PAGE_H_
#define SRC_WIZARD_PAGE_H_

#include <QVBoxLayout>
#include <QWizard>

#include "wizard_header.h"

class GondarWizard;

namespace gondar {

// For now this class is almost the same as a QWizardPage, but it'll
// be a helpful stepping stone for replacing QWizard with an
// implementation better tailored to our needs.
//
// The one minor piece of functionality it provides is that wizard()
// returns GondarWizard directly, rather than a generic QWizard
// pointer.
class WizardPage : public QWidget {
  Q_OBJECT

 public:
  explicit WizardPage(QWidget* parent = nullptr);

  virtual void initializePage();
  virtual int nextId() const;

  void setWizard(GondarWizard* wizard);

 signals:
  void completeChanged();

 protected:
  GondarWizard* wizard() const;

  void setButtonText(QWizard::WizardButton button, const QString& text);
  void setLayout(QLayout* layout);
  void setTitle(const QString& text);
  void setSubTitle(const QString& text);

  virtual bool isComplete() const;
  virtual bool validatePage();

 private:
  GondarWizard* wizard_ = nullptr;

  QVBoxLayout layout_;
  WizardHeader header_;
  QWidget center_;
};
}

#endif  // SRC_WIZARD_PAGE_H_
