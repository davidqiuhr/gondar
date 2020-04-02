// Copyright 2020 Neverware
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

#ifndef SRC_FEEDBACK_DIALOG_H_
#define SRC_FEEDBACK_DIALOG_H_

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "gondarwizard.h"

namespace gondar {

class FeedbackDialog : public QDialog {
  Q_OBJECT

 public:
  FeedbackDialog(GondarWizard & wizard_in);
  void submit();
  void maybeEnableSubmit();

 private:
  void handleReply(QNetworkReply* reply);
  QLabel feedback_label_;
  QTextEdit feedback_field_;
  QNetworkAccessManager network_manager_;
  QPushButton submit_button_;
  QVBoxLayout layout_;
  GondarWizard* wizard;
};
}  // namespace gondar

#endif  // SRC_FEEDBACK_DIALOG_H_
