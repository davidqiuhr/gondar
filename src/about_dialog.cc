// Copyright 2017 Neverware

#include "about_dialog.h"

#include <stdexcept>

#include <QFile>

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

  layout_.addWidget(&about_label_);
  layout_.addWidget(&license_text_browser_);
  layout_.addWidget(&close_button_, 0, Qt::AlignRight);
  setLayout(&layout_);
  setMinimumWidth(500);
  setWindowTitle(tr("CloudReady USB Creator"));
}
}
