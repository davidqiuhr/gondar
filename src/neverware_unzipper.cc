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

#include "neverware_unzipper.h"

#include <stdexcept>

#include <QDir>

#include "unzip.h"

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

#include "log.h"

// Defined in minizip/miniunz.c
extern "C" {
int miniunz_extract_currentfile(unzFile uf,
                                int opt_extract_without_path,
                                int* popt_overwrite,
                                const char* password);
}

namespace {

// RAII class for changing the current working directory. When it goes
// out of scope the working directory is reset.
class ScopedWorkingDirectory {
 public:
  ScopedWorkingDirectory() : orig_dir_(QDir::current()) {}

  ~ScopedWorkingDirectory() { setCwd(orig_dir_); }

  static void setCwd(const QDir& dir) {
    const QString path = dir.absolutePath();
    if (QDir::setCurrent(path)) {
      LOG_INFO << "set current working directory to " << path;
    } else {
      LOG_ERROR << "failed to set current working directory to " << path;
    }
  }

 private:
  const QDir orig_dir_;
};

class ZipError : std::runtime_error {
 public:
  ZipError(const std::string& what) : std::runtime_error(what) {}
};

class ZipFile {
  ZipFile& operator=(ZipFile&) = delete;
  ZipFile(ZipFile&) = delete;

 public:
  // Open a file for unzipping. Throw a ZipError if the open fails, so
  // the object is never partially constructed.
  explicit ZipFile(const QFileInfo& zipfile_info)
      : zipfile_info_(zipfile_info), file_(open(zipfile_info)) {}

  ~ZipFile() {
    const auto rc = unzClose(file_);
    if (rc != UNZ_OK) {
      LOG_ERROR << "unzClose failed: " << rc;
    }
  }

  // Extract the first file in the zip in the same directory as the
  // zipfile. Throw a ZipError if anything goes wrong.
  QFileInfo extractFirstFile() {
    const QString firstFileName = goToFirstFile();

    const int opt_extract_without_path = 1;
    int opt_overwrite = 1;
    const char* password = nullptr;

    {
      ScopedWorkingDirectory scoped_wd;
      scoped_wd.setCwd(zipfile_info_.absolutePath());

      const auto rc = miniunz_extract_currentfile(
          file_, opt_extract_without_path, &opt_overwrite, password);
      if (rc != UNZ_OK) {
        LOG_ERROR << "miniunz_extract_currentfile failed: " << rc;
        throw ZipError("miniunz_extract_currentfile failed");
      }
    }

    return zipfile_info_.absoluteDir().absoluteFilePath(firstFileName);
  }

 private:
  static unzFile open(const QFileInfo zipfile_info) {
    const std::string path = zipfile_info.absoluteFilePath().toStdString();
    LOG_INFO << "opening zipfile " << path;

#ifdef USEWIN32IOAPI
    static zlib_filefunc64_def ffunc;
    fill_win32_filefunc64A(&ffunc);
    unzFile file = unzOpen2_64(path.c_str(), &ffunc);
#else
    unzFile file = unzOpen64(path.c_str());
#endif

    if (!file) {
      LOG_ERROR << "failed to open zipfile: " << path;
      throw ZipError("error opening " + path);
    }

    return file;
  }

  // Move to the first file in the zip and get its name. Throw a
  // ZipError if anything goes wrong.
  QString goToFirstFile() {
    constexpr int FILENAME_BUFFER_SIZE = 256;
    char filename[FILENAME_BUFFER_SIZE] = {};
    unz_file_info64 file_info = {};

    void* extrafield = nullptr;
    const uint16_t extrafield_size = 0;
    char* comment = nullptr;
    const uint16_t comment_size = 0;
    const auto rc =
        unzGoToFirstFile2(file_, &file_info, filename, FILENAME_BUFFER_SIZE,
                          extrafield, extrafield_size, comment, comment_size);

    if (rc != UNZ_OK) {
      LOG_ERROR << "unzGoToFirstFile2 failed: " << rc;
      throw ZipError("unzGoToFirstFile2 failed");
    }

    return filename;
  }

  const QFileInfo zipfile_info_;
  unzFile file_;
};

}  // namespace

QFileInfo neverware_unzip(const QFileInfo& input_file) {
  return ZipFile(input_file).extractFirstFile();
}
