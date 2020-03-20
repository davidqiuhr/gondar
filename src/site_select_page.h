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

#ifndef SRC_SITE_SELECT_PAGE_H_
#define SRC_SITE_SELECT_PAGE_H_

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "gondarsite.h"
#include "wizard_page.h"

class FindDialog : public QDialog {
  Q_OBJECT
 public:
  FindDialog(QWidget *parent = nullptr);
  QString getFindText();
  QPushButton findButton;

private:
  QLabel findLabel;
  QLineEdit lineEdit;
  QHBoxLayout layout;
};

class SiteSelectPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  explicit SiteSelectPage(QWidget* parent = 0);
  QString getFindText();
  FindDialog find;

 protected:
  void initializePage() override;
  bool validatePage() override;

 private:
  QListWidget sitesEntries;
  QVBoxLayout layout;
  void filterSites();
};

#endif  // SRC_SITE_SELECT_PAGE_H_
