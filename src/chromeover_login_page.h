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

#ifndef CHROMEOVER_LOGIN_PAGE_H
#define CHROMEOVER_LOGIN_PAGE_H

#include <vector>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "meepo.h"
#include "wizard_page.h"

class ChromeoverLoginPage : public gondar::WizardPage {
  Q_OBJECT

 public:
  ChromeoverLoginPage(QWidget* parent = 0);
  int nextId() const override;

 protected:
  bool validatePage() override;

 private:
  void handleMeepoFinished();

  gondar::Meepo meepo_;

  QGridLayout layout;
  QLineEdit usernameLineEdit;
  QLabel usernameLineEditLabel;
  QLineEdit passwordLineEdit;
  QLabel passwordLineEditLabel;
  QLabel meanWordsLabel;
  bool finished;
  bool started;
};

#endif
