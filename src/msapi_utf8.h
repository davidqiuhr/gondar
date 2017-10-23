/*
 * MSAPI_UTF8: Common API calls using UTF-8 strings
 * Compensating for what Microsoft should have done a long long time ago.
 * Also see http://utf8everywhere.org/
 *
 * Copyright © 2010-2017 Pete Batard <pete@akeo.ie>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SRC_MSAPI_UTF8_H_
#define SRC_MSAPI_UTF8_H_

#include <setupapi.h>
#include <windows.h>
// TODO: necessary?
#include <setupapi.h>
#include <usbioctl.h>
#include <versionhelpers.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
// ?

#ifdef __cplusplus
extern "C" {
#endif

#define wchar_to_utf8_no_alloc(wsrc, dest, dest_size) \
  WideCharToMultiByte(CP_UTF8, 0, wsrc, -1, dest, dest_size, NULL, NULL)
#define utf8_to_wchar_no_alloc(src, wdest, wdest_size) \
  MultiByteToWideChar(CP_UTF8, 0, src, -1, wdest, wdest_size)

#define sfree(p)        \
  do {                  \
    if (p != NULL) {    \
      free((void*)(p)); \
      p = NULL;         \
    }                   \
  } while (0)
#define wconvert(p) wchar_t* w##p = utf8_to_wchar(p)
#define walloc(p, size) \
  wchar_t* w##p = (p == NULL) ? NULL : (wchar_t*)calloc(size, sizeof(wchar_t))
#define wfree(p) sfree(w##p)

/*
 * Converts an UTF-16 string to UTF8 (allocate returned string)
 * Returns NULL on error
 */
static __inline char* wchar_to_utf8(const wchar_t* wstr) {
  int size = 0;
  char* str = NULL;

  // Find out the size we need to allocate for our converted string
  size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
  if (size <= 1)  // An empty string would be size 1
    return NULL;

  if ((str = (char*)calloc(size, 1)) == NULL)
    return NULL;

  if (wchar_to_utf8_no_alloc(wstr, str, size) != size) {
    sfree(str);
    return NULL;
  }

  return str;
}

/*
 * Converts an UTF8 string to UTF-16 (allocate returned string)
 * Returns NULL on error
 */
static __inline wchar_t* utf8_to_wchar(const char* str) {
  int size = 0;
  wchar_t* wstr = NULL;

  if (str == NULL)
    return NULL;

  // Find out the size we need to allocate for our converted string
  size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
  if (size <= 1)  // An empty string would be size 1
    return NULL;

  if ((wstr = (wchar_t*)calloc(size, sizeof(wchar_t))) == NULL)
    return NULL;

  if (utf8_to_wchar_no_alloc(str, wstr, size) != size) {
    sfree(wstr);
    return NULL;
  }
  return wstr;
}

static __inline int LoadStringU(HINSTANCE hInstance,
                                UINT uID,
                                LPSTR lpBuffer,
                                int nBufferMax) {
  int ret;
  DWORD err = ERROR_INVALID_DATA;
  if (nBufferMax == 0) {
    // read-only pointer to resource mode is not supported
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  walloc(lpBuffer, nBufferMax);
  ret = LoadStringW(hInstance, uID, wlpBuffer, nBufferMax);
  err = GetLastError();
  if ((ret > 0) &&
      ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, nBufferMax)) == 0)) {
    err = GetLastError();
  }
  wfree(lpBuffer);
  SetLastError(err);
  return ret;
}

static __inline HMODULE LoadLibraryU(LPCSTR lpFileName) {
  HMODULE ret;
  DWORD err = ERROR_INVALID_DATA;
  wconvert(lpFileName);
  ret = LoadLibraryW(wlpFileName);
  err = GetLastError();
  wfree(lpFileName);
  SetLastError(err);
  return ret;
}

static __inline int GetWindowTextU(HWND hWnd, char* lpString, int nMaxCount) {
  int ret = 0;
  DWORD err = ERROR_INVALID_DATA;
  walloc(lpString, nMaxCount);
  ret = GetWindowTextW(hWnd, wlpString, nMaxCount);
  err = GetLastError();
  if ((ret != 0) &&
      ((ret = wchar_to_utf8_no_alloc(wlpString, lpString, nMaxCount)) == 0)) {
    err = GetLastError();
  }
  wfree(lpString);
  SetLastError(err);
  return ret;
}

static __inline HANDLE CreateFileU(const char* lpFileName,
                                   DWORD dwDesiredAccess,
                                   DWORD dwShareMode,
                                   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                   DWORD dwCreationDisposition,
                                   DWORD dwFlagsAndAttributes,
                                   HANDLE hTemplateFile) {
  HANDLE ret = INVALID_HANDLE_VALUE;
  DWORD err = ERROR_INVALID_DATA;
  wconvert(lpFileName);
  ret = CreateFileW(wlpFileName, dwDesiredAccess, dwShareMode,
                    lpSecurityAttributes, dwCreationDisposition,
                    dwFlagsAndAttributes, hTemplateFile);
  err = GetLastError();
  wfree(lpFileName);
  SetLastError(err);
  return ret;
}

// The following expects PropertyBuffer to contain a single Unicode string
static __inline bool SetupDiGetDeviceRegistryPropertyU(
    HDEVINFO DeviceInfoSet,
    PSP_DEVINFO_DATA DeviceInfoData,
    DWORD Property,
    PDWORD PropertyRegDataType,
    PBYTE PropertyBuffer,
    DWORD PropertyBufferSize,
    PDWORD RequiredSize) {
  bool ret = false;
  DWORD err = ERROR_INVALID_DATA;
  walloc(PropertyBuffer, PropertyBufferSize);

  ret = SetupDiGetDeviceRegistryPropertyW(
      DeviceInfoSet, DeviceInfoData, Property, PropertyRegDataType,
      (PBYTE)wPropertyBuffer, PropertyBufferSize, RequiredSize);
  err = GetLastError();
  if ((ret != 0) &&
      (wchar_to_utf8_no_alloc(wPropertyBuffer, (char*)(uintptr_t)PropertyBuffer,
                              PropertyBufferSize) == 0)) {
    err = GetLastError();
    ret = false;
  }
  wfree(PropertyBuffer);
  SetLastError(err);
  return ret;
}

static __inline bool GetVolumeInformationU(LPCSTR lpRootPathName,
                                           LPSTR lpVolumeNameBuffer,
                                           DWORD nVolumeNameSize,
                                           LPDWORD lpVolumeSerialNumber,
                                           LPDWORD lpMaximumComponentLength,
                                           LPDWORD lpFileSystemFlags,
                                           LPSTR lpFileSystemNameBuffer,
                                           DWORD nFileSystemNameSize) {
  bool ret = false;
  DWORD err = ERROR_INVALID_DATA;
  wconvert(lpRootPathName);
  walloc(lpVolumeNameBuffer, nVolumeNameSize);
  walloc(lpFileSystemNameBuffer, nFileSystemNameSize);

  ret = GetVolumeInformationW(wlpRootPathName, wlpVolumeNameBuffer,
                              nVolumeNameSize, lpVolumeSerialNumber,
                              lpMaximumComponentLength, lpFileSystemFlags,
                              wlpFileSystemNameBuffer, nFileSystemNameSize);
  err = GetLastError();
  if (ret) {
    if (((lpVolumeNameBuffer != NULL) &&
         (wchar_to_utf8_no_alloc(wlpVolumeNameBuffer, lpVolumeNameBuffer,
                                 nVolumeNameSize) == 0)) ||
        ((lpFileSystemNameBuffer != NULL) &&
         (wchar_to_utf8_no_alloc(wlpFileSystemNameBuffer,
                                 lpFileSystemNameBuffer,
                                 nFileSystemNameSize) == 0))) {
      err = GetLastError();
      ret = false;
    }
  }
  wfree(lpVolumeNameBuffer);
  wfree(lpFileSystemNameBuffer);
  wfree(lpRootPathName);
  SetLastError(err);
  return ret;
}

#ifdef __cplusplus
}
#endif

#endif /* SRC_MSAPI_UTF8_H_ */
