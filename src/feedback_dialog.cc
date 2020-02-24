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

#include "feedback_dialog.h"

#include <stdexcept>

#include <QFile>

#include "log.h"
#include "util.h"

namespace gondar {

FeedbackDialog::FeedbackDialog() {
  feedback_label_.setText("Feedback");

  submit_button_.setText(tr("&Submit"));
  connect(&submit_button_, &QPushButton::clicked, this, &FeedbackDialog::submit);

  layout_.addWidget(&feedback_label_);
  layout_.addWidget(&title_);
  layout_.addWidget(&details_);
  layout_.addWidget(&submit_button_, 0, Qt::AlignRight);
  setLayout(&layout_);
  setMinimumWidth(500);
  setWindowTitle(tr("CloudReady USB Maker"));
}

void FeedbackDialog::submit() {
  LOG_WARNING << "the feedback was accepted";
  accept();
}
}  // namespace gondar
