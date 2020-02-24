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

#ifndef SRC_FEEDBACK_DIALOG_H_
#define SRC_FEEDBACK_DIALOG_H_

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace gondar {

class FeedbackDialog : public QDialog {
  Q_OBJECT

 public:
  FeedbackDialog();
  void submit();

 private:
  QVBoxLayout layout_;
  QLabel feedback_label_;
  QTextEdit title_;
  QTextEdit details_;
  QPushButton submit_button_;
};
}  // namespace gondar

#endif  // SRC_FEEDBACK_DIALOG_H_
