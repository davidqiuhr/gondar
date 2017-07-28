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

#include "unzipthread.h"

#include <QtWidgets>

#include "log.h"
#include "neverware_unzipper.h"

UnzipThread::UnzipThread(const QFileInfo& input, QObject* parent)
    : QThread(parent), inputFile(input) {}

UnzipThread::~UnzipThread() {}

const QString& UnzipThread::getFileName() const {
  return filename;
}
void UnzipThread::run() {
  try {
    const QFileInfo binfile = neverware_unzip(inputFile);
    filename = binfile.absoluteFilePath();
  } catch (const std::exception& exc) {
    LOG_ERROR << "unzip failed: " << exc.what();
    filename = QString();
  }
  qDebug() << "worker thread says complete";
}
