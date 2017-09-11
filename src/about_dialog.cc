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

#include "about_dialog.h"

#include <stdexcept>

#include <QFile>

#include "util.h"

namespace gondar {

namespace {

QString readTextFile(const QString& path) {
  QFile file(path);
  if (!file.open(QFile::ReadOnly)) {
    throw std::runtime_error("error opening file for reading");
  }

  return QString::fromUtf8(file.readAll());
}

}  // namespace

AboutDialog::AboutDialog() {
  about_label_.setOpenExternalLinks(true);
  about_label_.setWordWrap(true);
  about_label_.setText(readTextFile(":/about.html"));

  license_text_browser_.setOpenExternalLinks(true);
  license_text_browser_.setHtml(readTextFile(":/gpl-3.0-standalone.html"));
  license_text_browser_.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  close_button_.setText(tr("&Close"));
  connect(&close_button_, &QPushButton::clicked, this, &AboutDialog::accept);

  QString version =  gondar::getGondarVersion();
  // if version is non-empty
  if (version.size() > 0) {
    version_label_.setText("Version " + version);
    layout_.addWidget(&version_label_);
  }
  layout_.addWidget(&about_label_);
  layout_.addWidget(&license_text_browser_);
  layout_.addWidget(&close_button_, 0, Qt::AlignRight);
  setLayout(&layout_);
  setMinimumWidth(500);
  setWindowTitle(tr("CloudReady USB Maker"));
}
}
