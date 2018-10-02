// Copyright 2018 Neverware
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

#include "mkfs.h"

#include <inttypes.h>
#include <windows.h>
#include <winioctl.h>  // for MEDIA_TYPE

#include "log.h"
#include "msapi_utf8.h"

HMODULE OpenedLibraryHandle = NULL;
static __inline HMODULE GetLibraryHandle() {
  return OpenedLibraryHandle;
}

/* Callback command types (some errorcode were filled from HPUSBFW V2.2.3 and
   their
   designation from
   msdn.microsoft.com/en-us/library/windows/desktop/aa819439.aspx */
typedef enum {
  FCC_PROGRESS,
  FCC_DONE_WITH_STRUCTURE,
  FCC_UNKNOWN2,
  FCC_INCOMPATIBLE_FILE_SYSTEM,
  FCC_UNKNOWN4,
  FCC_UNKNOWN5,
  FCC_ACCESS_DENIED,
  FCC_MEDIA_WRITE_PROTECTED,
  FCC_VOLUME_IN_USE,
  FCC_CANT_QUICK_FORMAT,
  FCC_UNKNOWNA,
  FCC_DONE,
  FCC_BAD_LABEL,
  FCC_UNKNOWND,
  FCC_OUTPUT,
  FCC_STRUCTURE_PROGRESS,
  FCC_CLUSTER_SIZE_TOO_SMALL,
  FCC_CLUSTER_SIZE_TOO_BIG,
  FCC_VOLUME_TOO_SMALL,
  FCC_VOLUME_TOO_BIG,
  FCC_NO_MEDIA_IN_DRIVE,
  FCC_UNKNOWN15,
  FCC_UNKNOWN16,
  FCC_UNKNOWN17,
  FCC_DEVICE_NOT_READY,
  FCC_CHECKDISK_PROGRESS,
  FCC_UNKNOWN1A,
  FCC_UNKNOWN1B,
  FCC_UNKNOWN1C,
  FCC_UNKNOWN1D,
  FCC_UNKNOWN1E,
  FCC_UNKNOWN1F,
  FCC_READ_ONLY_MODE,
} FILE_SYSTEM_CALLBACK_COMMAND;

typedef BOOLEAN(__stdcall* FILE_SYSTEM_CALLBACK)(
    FILE_SYSTEM_CALLBACK_COMMAND Command,
    ULONG Action,
    PVOID pData);

/* Parameter names aligned to
   http://msdn.microsoft.com/en-us/library/windows/desktop/aa819439.aspx */
typedef VOID(WINAPI* FormatEx_t)(WCHAR* DriveRoot,
                                 MEDIA_TYPE MediaType,  // See WinIoCtl.h
                                 WCHAR* FileSystemTypeName,
                                 WCHAR* Label,
                                 BOOL QuickFormat,
                                 ULONG DesiredUnitAllocationSize,
                                 FILE_SYSTEM_CALLBACK Callback);
FormatEx_t pfFormatEx = NULL;

/*
 * FormatEx callback. Return FALSE to halt operations
 */
static BOOLEAN __stdcall FormatExCallback(FILE_SYSTEM_CALLBACK_COMMAND Command,
                                          DWORD,
                                          PVOID) {
  switch (Command) {
    case FCC_PROGRESS:
      LOG_INFO << "still formatting drive...";
      break;
    case FCC_CLUSTER_SIZE_TOO_SMALL:
      LOG_WARNING << "cluster size too small";
      break;
    case FCC_CLUSTER_SIZE_TOO_BIG:
      LOG_WARNING << "cluster size too big";
      break;
    case FCC_DONE:
      LOG_INFO << "finished formatting drive";
      break;
    case FCC_READ_ONLY_MODE:
      LOG_WARNING << "read-only mode";
      break;
    case FCC_DEVICE_NOT_READY:
      LOG_WARNING << "device not ready";
      break;
    default:
      LOG_WARNING << "unknown callback case";
      break;
  }
  return true;
}

void makeFilesystem(char* logical_path) {
  LOG_WARNING << "making filesystem...";
  // LoadLibrary("fmifs.dll") appears to change the locale, which can lead to
  // problems with tolower(). Make sure we restore the locale. For more details,
  // see http://comments.gmane.org/gmane.comp.gnu.mingw.user/39300
  char* locale = setlocale(LC_ALL, NULL);
  pfFormatEx =
      (FormatEx_t)GetProcAddress(LoadLibraryA("fmifs.dll"), "FormatEx");
  setlocale(LC_ALL, locale);

  wchar_t* logical_path_windows = utf8_to_wchar(logical_path);
  std::wstring fat32str = L"FAT32";
  std::wstring emptystr = L"";
  // TODO: is it ok to cast from const wchar * to wchar *?
  wchar_t* fat32 = (wchar_t*)fat32str.c_str();
  wchar_t* empty = (wchar_t*)emptystr.c_str();
  // use 4096 as cluster size as it is generally considered a sane default
  // see:
  // https://superuser.com/questions/1286135/what-allocation-unit-size-to-use-when-formatting-a-usb-flash-drive-in-fat32/1287233
  pfFormatEx(logical_path_windows,
             RemovableMedia,  // MEDIA_TYPE
             // L"FAT32",
             // L"",
             fat32, empty,
             true,  // quick format
             4096,  // cluster size
             FormatExCallback);
  LOG_INFO << "sent request to make filesystem...";
}

void deleteLibrary() {
  if (OpenedLibraryHandle) {
    FreeLibrary(OpenedLibraryHandle);
  }
}
