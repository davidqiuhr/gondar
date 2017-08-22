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

#ifndef SRC_IMAGE_METADATA_H_
#define SRC_IMAGE_METADATA_H_

#include <QFileInfo>
#include <QUrl>

#include "option.h"
#include "sha512.h"

namespace gondar {

class ImageMetadata {
 public:
  const QUrl& url() const;
  void setUrl(const QUrl& url);

 private:
  QUrl url_;
  QFileInfo file_info_;
  Option<Sha512> sha_512_;
};

}  // namespace gondar

#endif  // SRC_IMAGE_METADATA_H_
