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

#include "update_check.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>

#include "log.h"
#include "util.h"

namespace gondar {

void UpdateCheck::start(QWidget* parent) {
  if (gondar::getGondarVersion().isEmpty()) {
    LOG_INFO << "Skipping latest version check for dev build";
    return;
  }

  if (parent_) {
    LOG_INFO << "update check already started";
    return;
  }

  parent_ = parent;

  static QNetworkAccessManager manager;
  connect(&manager, &QNetworkAccessManager::finished, this,
          &UpdateCheck::handleReply);

  manager.get(QNetworkRequest(
      // TODO: create actual endpoint file
      QUrl("http://neverware.com/hypothetical-latest-gondar-release-file")));
}

void UpdateCheck::handleReply(QNetworkReply* reply) {
  const QString latestVersionString = reply->readAll();
  const double latestVersion = latestVersionString.toDouble();
  const double currentVersion = gondar::getGondarVersion().toDouble();
  LOG_INFO << "Latest version: " << latestVersion
           << ", currentVersion: " << currentVersion;
  if (currentVersion < latestVersion) {
    showMessage(latestVersionString);
  }
}

void UpdateCheck::showMessage(const QString& latestVersionString) {
  const QString baseUrl = "https://usb-maker-downloads.neverware.com/stable/";
  const QString filename = "cloudready-usb-maker.exe";
  QString product;
  if (gondar::isChromeover()) {
    product = "cloudready/";
  } else {
    product = "cloudready-free/";
  }
  const QString downloadUrl = baseUrl + product + filename;
  QMessageBox box(QMessageBox::Information, "Update Available",
                  "A new version of CloudReady USB Maker is now available.");
  box.setInformativeText(
      QString("Click 'Download Update' to download version %2 in your web "
              "browser.  When your download starts you can close CloudReady "
              "USB Maker.  To complete the upgrade just launch the new "
              "version.")
          .arg(latestVersionString));
  auto reject = box.addButton("&Not Now", QMessageBox::RejectRole);
  auto accept = box.addButton("&Download Update", QMessageBox::AcceptRole);
  box.setEscapeButton(reject);
  box.setDefaultButton(accept);
  box.exec();
  if (box.clickedButton() == accept) {
    QDesktopServices::openUrl(downloadUrl);
  }
}
}
