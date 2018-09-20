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
#include <windows.h>
#include <stdio.h>
#include <shlobj.h>
#include <ctype.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <setupapi.h>
#include <direct.h>
#include <share.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <psapi.h>

#pragma once
#if defined(_MSC_VER)
// disable VS2012 Code Analysis warnings that are intentional
#pragma warning(disable: 6387)	// Don't care about bad params
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define _LTEXT(txt) L##txt
#define LTEXT(txt) _LTEXT(txt)

#define wchar_to_utf8_no_alloc(wsrc, dest, dest_size) \
	WideCharToMultiByte(CP_UTF8, 0, wsrc, -1, dest, dest_size, NULL, NULL)
#define utf8_to_wchar_no_alloc(src, wdest, wdest_size) \
	MultiByteToWideChar(CP_UTF8, 0, src, -1, wdest, wdest_size)
#define Edit_ReplaceSelU(hCtrl, str) ((void)SendMessageLU(hCtrl, EM_REPLACESEL, (WPARAM)FALSE, str))
#define ComboBox_AddStringU(hCtrl, str) ((int)(DWORD)SendMessageLU(hCtrl, CB_ADDSTRING, (WPARAM)FALSE, str))
#define ComboBox_InsertStringU(hCtrl, index, str) ((int)(DWORD)SendMessageLU(hCtrl, CB_INSERTSTRING, (WPARAM)index, str))
#define ComboBox_GetTextU(hCtrl, str, max_str) GetWindowTextU(hCtrl, str, max_str)
#define GetSaveFileNameU(p) GetOpenSaveFileNameU(p, TRUE)
#define GetOpenFileNameU(p) GetOpenSaveFileNameU(p, FALSE)
#define ListView_SetItemTextU(hwndLV,i,iSubItem_,pszText_) { LVITEMW _ms_wlvi; _ms_wlvi.iSubItem = iSubItem_; \
	_ms_wlvi.pszText = utf8_to_wchar(pszText_); \
	SNDMSG((hwndLV),LVM_SETITEMTEXTW,(WPARAM)(i),(LPARAM)&_ms_wlvi); sfree(_ms_wlvi.pszText);}

// Never ever use isdigit() or isspace(), etc. on UTF-8 strings!
// These calls take an int and char is signed so MS compilers will produce an assert error on anything that's > 0x80
#define isasciiU(c) isascii((unsigned char)(c))
#define iscntrlU(c) iscntrl((unsigned char)(c))
#define isdigitU(c) isdigit((unsigned char)(c))
#define isspaceU(c) isspace((unsigned char)(c))
#define isxdigitU(c) isxdigit((unsigned char)(c))
// NB: other issomething() calls are not implemented as they may require multibyte UTF-8 sequences to be converted

#define sfree(p) do {if (p != NULL) {free((void*)(p)); p = NULL;}} while(0)
#define wconvert(p)     wchar_t* w ## p = utf8_to_wchar(p)
#define walloc(p, size) wchar_t* w ## p = (p == NULL)?NULL:(wchar_t*)calloc(size, sizeof(wchar_t))
#define wfree(p) sfree(w ## p)

/*
 * Converts an UTF-16 string to UTF8 (allocate returned string)
 * Returns NULL on error
 */
static __inline char* wchar_to_utf8(const wchar_t* wstr)
{
	int size = 0;
	char* str = NULL;

	// Find out the size we need to allocate for our converted string
	size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (size <= 1)	// An empty string would be size 1
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
static __inline wchar_t* utf8_to_wchar(const char* str)
{
	int size = 0;
	wchar_t* wstr = NULL;

	if (str == NULL)
		return NULL;

	// Find out the size we need to allocate for our converted string
	size = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if (size <= 1)	// An empty string would be size 1
		return NULL;

	if ((wstr = (wchar_t*)calloc(size, sizeof(wchar_t))) == NULL)
		return NULL;

	if (utf8_to_wchar_no_alloc(str, wstr, size) != size) {
		sfree(wstr);
		return NULL;
	}
	return wstr;
}

/*
* Converts an non NUL-terminated UTF-16 string of length len to UTF8 (allocate returned string)
* Returns NULL on error
*/
static __inline char* wchar_len_to_utf8(const wchar_t* wstr, int wlen)
{
	int size = 0;
	char* str = NULL;

	// Find out the size we need to allocate for our converted string
	size = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
	if (size <= 1)	// An empty string would be size 1
		return NULL;

	if ((str = (char*)calloc(size, 1)) == NULL)
		return NULL;

	if (WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, str, size, NULL, NULL) != size) {
		sfree(str);
		return NULL;
	}

	return str;
}

static __inline DWORD FormatMessageU(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId,
									 DWORD dwLanguageId, char* lpBuffer, DWORD nSize, va_list *Arguments)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpBuffer, nSize);
	ret = FormatMessageW(dwFlags, lpSource, dwMessageId, dwLanguageId, wlpBuffer, nSize, Arguments);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, nSize)) == 0)) {
		err = GetLastError();
		ret = 0;
	}
	wfree(lpBuffer);
	SetLastError(err);
	return ret;
}

// SendMessage, with LPARAM as UTF-8 string
static __inline LRESULT SendMessageLU(HWND hWnd, UINT Msg, WPARAM wParam, const char* lParam)
{
	LRESULT ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lParam);
	ret = SendMessageW(hWnd, Msg, wParam, (LPARAM)wlParam);
	err = GetLastError();
	wfree(lParam);
	SetLastError(err);
	return ret;
}

static __inline int DrawTextExU(HDC hDC, LPCSTR lpchText, int nCount, LPRECT lpRect, UINT uFormat, LPDRAWTEXTPARAMS lpDTParams)
{
	int ret;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpchText);
	ret = DrawTextExW(hDC, wlpchText, nCount, lpRect, uFormat, lpDTParams);
	err = GetLastError();
	wfree(lpchText);
	SetLastError(err);
	return ret;
}

static __inline BOOL SHGetPathFromIDListU(LPCITEMIDLIST pidl, char* pszPath)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(pszPath, MAX_PATH);
	ret = SHGetPathFromIDListW(pidl, wpszPath);
	err = GetLastError();
	if ((ret) && (wchar_to_utf8_no_alloc(wpszPath, pszPath, MAX_PATH) == 0)) {
		err = GetLastError();
		ret = FALSE;
	}
	wfree(pszPath);
	SetLastError(err);
	return ret;
}

static __inline HWND CreateWindowU(char* lpClassName, char* lpWindowName,
	DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,
	HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND ret = NULL;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpClassName);
	wconvert(lpWindowName);
	ret = CreateWindowW(wlpClassName, wlpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	err = GetLastError();
	wfree(lpClassName);
	wfree(lpWindowName);
	SetLastError(err);
	return ret;
}

static __inline HWND CreateWindowExU(DWORD dwExStyle, char* lpClassName, char* lpWindowName,
	DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
	HINSTANCE hInstance, LPVOID lpParam)
{
	HWND ret = NULL;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpClassName);
	wconvert(lpWindowName);
	ret = CreateWindowExW(dwExStyle, wlpClassName, wlpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	err = GetLastError();
	wfree(lpClassName);
	wfree(lpWindowName);
	SetLastError(err);
	return ret;
}

static __inline int MessageBoxU(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int ret;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpText);
	wconvert(lpCaption);
	ret = MessageBoxW(hWnd, wlpText, wlpCaption, uType);
	err = GetLastError();
	wfree(lpText);
	wfree(lpCaption);
	SetLastError(err);
	return ret;
}

static __inline int MessageBoxExU(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId)
{
	int ret;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpText);
	wconvert(lpCaption);
	ret = MessageBoxExW(hWnd, wlpText, wlpCaption, uType, wLanguageId);
	err = GetLastError();
	wfree(lpText);
	wfree(lpCaption);
	SetLastError(err);
	return ret;
}

static __inline int LoadStringU(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int nBufferMax)
{
	int ret;
	DWORD err = ERROR_INVALID_DATA;
	if (nBufferMax == 0) {
		// read-only pointer to resource mode is not supported
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	// coverity[returned_null]
	walloc(lpBuffer, nBufferMax);
	ret = LoadStringW(hInstance, uID, wlpBuffer, nBufferMax);
	err = GetLastError();
	if ((ret > 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, nBufferMax)) == 0)) {
		err = GetLastError();
	}
	wfree(lpBuffer);
	SetLastError(err);
	return ret;
}

static __inline HMODULE LoadLibraryU(LPCSTR lpFileName)
{
	HMODULE ret;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpFileName);
	ret = LoadLibraryW(wlpFileName);
	err = GetLastError();
	wfree(lpFileName);
	SetLastError(err);
	return ret;
}

static __inline int DrawTextU(HDC hDC, LPCSTR lpText, int nCount, LPRECT lpRect, UINT uFormat)
{
	int ret;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpText);
	ret = DrawTextW(hDC, wlpText, nCount, lpRect, uFormat);
	err = GetLastError();
	wfree(lpText);
	SetLastError(err);
	return ret;
}

static __inline int GetWindowTextU(HWND hWnd, char* lpString, int nMaxCount)
{
	int ret = 0;
	DWORD err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpString, nMaxCount);
	ret = GetWindowTextW(hWnd, wlpString, nMaxCount);
	err = GetLastError();
	if ( (ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpString, lpString, nMaxCount)) == 0) ) {
		err = GetLastError();
	}
	wfree(lpString);
	SetLastError(err);
	return ret;
}

static __inline BOOL SetWindowTextU(HWND hWnd, const char* lpString)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpString);
	ret = SetWindowTextW(hWnd, wlpString);
	err = GetLastError();
	wfree(lpString);
	SetLastError(err);
	return ret;
}

static __inline UINT GetDlgItemTextU(HWND hDlg, int nIDDlgItem, char* lpString, int nMaxCount)
{
	UINT ret = 0;
	DWORD err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpString, nMaxCount);
	ret = GetDlgItemTextW(hDlg, nIDDlgItem, wlpString, nMaxCount);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpString, lpString, nMaxCount)) == 0)) {
		err = GetLastError();
	}
	wfree(lpString);
	SetLastError(err);
	return ret;
}

static __inline BOOL SetDlgItemTextU(HWND hDlg, int nIDDlgItem, const char* lpString)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpString);
	ret = SetDlgItemTextW(hDlg, nIDDlgItem, wlpString);
	err = GetLastError();
	wfree(lpString);
	SetLastError(err);
	return ret;
}

static __inline BOOL InsertMenuU(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, const char* lpNewItem)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpNewItem);
	ret = InsertMenuW(hMenu, uPosition, uFlags, uIDNewItem, wlpNewItem);
	err = GetLastError();
	wfree(lpNewItem);
	SetLastError(err);
	return ret;
}

static __inline int ComboBox_GetLBTextU(HWND hCtrl, int index, char* lpString)
{
	int size;
	DWORD err = ERROR_INVALID_DATA;
	wchar_t* wlpString;
	if (lpString == NULL)
		return CB_ERR;
	size = (int)SendMessageW(hCtrl, CB_GETLBTEXTLEN, (WPARAM)index, (LPARAM)0);
	if (size < 0)
		return size;
	wlpString = (wchar_t*)calloc(size+1, sizeof(wchar_t));
	size = (int)SendMessageW(hCtrl, CB_GETLBTEXT, (WPARAM)index, (LPARAM)wlpString);
	err = GetLastError();
	if (size > 0)
		wchar_to_utf8_no_alloc(wlpString, lpString, size+1);
	wfree(lpString);
	SetLastError(err);
	return size;
}

static __inline DWORD CharUpperBuffU(char* lpString, DWORD len)
{
	DWORD ret;
	wchar_t *wlpString = (wchar_t*)calloc(len, sizeof(wchar_t));
	if (wlpString == NULL)
		return 0;
	utf8_to_wchar_no_alloc(lpString, wlpString, len);
	ret = CharUpperBuffW(wlpString, len);
	wchar_to_utf8_no_alloc(wlpString, lpString, len);
	free(wlpString);
	return ret;
}

static __inline HANDLE CreateFileU(const char* lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
								   LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
								   DWORD dwFlagsAndAttributes,  HANDLE hTemplateFile)
{
	HANDLE ret = INVALID_HANDLE_VALUE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpFileName);
	ret = CreateFileW(wlpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	err = GetLastError();
	wfree(lpFileName);
	SetLastError(err);
	return ret;
}

static __inline BOOL CopyFileU(const char* lpExistingFileName, const char* lpNewFileName, BOOL bFailIfExists)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpExistingFileName);
	wconvert(lpNewFileName);
	ret = CopyFileW(wlpExistingFileName, wlpNewFileName, bFailIfExists);
	err = GetLastError();
	wfree(lpExistingFileName);
	wfree(lpNewFileName);
	SetLastError(err);
	return ret;
}

static __inline BOOL DeleteFileU(const char* lpFileName)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpFileName);
	ret = DeleteFileW(wlpFileName);
	err = GetLastError();
	wfree(lpFileName);
	SetLastError(err);
	return ret;
}

static __inline BOOL PathFileExistsU(char* szPath)
{
	BOOL ret;
	wconvert(szPath);
	ret = PathFileExistsW(wszPath);
	wfree(szPath);
	return ret;
}

static __inline int PathGetDriveNumberU(char* lpPath)
{
	int ret = 0;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpPath);
	ret = PathGetDriveNumberW(wlpPath);
	err = GetLastError();
	wfree(lpPath);
	SetLastError(err);
	return ret;
}

// This one is tricky since we can't blindly convert a
// UTF-16 position to a UTF-8 one. So we do it manually.
static __inline const char* PathFindFileNameU(const char* szPath)
{
	size_t i;
	if (szPath == NULL)
		return NULL;
	for (i = strlen(szPath); i != 0; i--) {
		if ((szPath[i] == '/') || (szPath[i] == '\\')) {
			i++;
			break;
		}
	}
	return &szPath[i];
}

// This function differs from regular GetTextExtentPoint in that it uses a zero terminated string
static __inline BOOL GetTextExtentPointU(HDC hdc, const char* lpString, LPSIZE lpSize)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpString);
	if (wlpString == NULL)
		return FALSE;
	ret = GetTextExtentPoint32W(hdc, wlpString, (int)wcslen(wlpString), lpSize);
	err = GetLastError();
	wfree(lpString);
	SetLastError(err);
	return ret;
}

static __inline DWORD GetCurrentDirectoryU(DWORD nBufferLength, char* lpBuffer)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpBuffer, nBufferLength);
	ret = GetCurrentDirectoryW(nBufferLength, wlpBuffer);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, nBufferLength)) == 0)) {
		err = GetLastError();
	}
	wfree(lpBuffer);
	SetLastError(err);
	return ret;
}

static __inline UINT GetSystemDirectoryU(char* lpBuffer, UINT uSize)
{
	UINT ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpBuffer, uSize);
	ret = GetSystemDirectoryW(wlpBuffer, uSize);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, uSize)) == 0)) {
		err = GetLastError();
	}
	wfree(lpBuffer);
	SetLastError(err);
	return ret;
}

static __inline UINT GetSystemWindowsDirectoryU(char* lpBuffer, UINT uSize)
{
	UINT ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpBuffer, uSize);
	ret = GetSystemWindowsDirectoryW(wlpBuffer, uSize);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, uSize)) == 0)) {
		err = GetLastError();
	}
	wfree(lpBuffer);
	SetLastError(err);
	return ret;
}

static __inline DWORD GetTempPathU(DWORD nBufferLength, char* lpBuffer)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpBuffer, nBufferLength);
	ret = GetTempPathW(nBufferLength, wlpBuffer);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, nBufferLength)) == 0)) {
		err = GetLastError();
	}
	wfree(lpBuffer);
	SetLastError(err);
	return ret;
}

static __inline DWORD GetTempFileNameU(char* lpPathName, char* lpPrefixString, UINT uUnique, char* lpTempFileName)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	wconvert(lpPathName);
	wconvert(lpPrefixString);
	// coverity[returned_null]
	walloc(lpTempFileName, MAX_PATH);
	ret = GetTempFileNameW(wlpPathName, wlpPrefixString, uUnique, wlpTempFileName);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpTempFileName, lpTempFileName, MAX_PATH)) == 0)) {
		err = GetLastError();
	}
	wfree(lpTempFileName);
	wfree(lpPrefixString);
	wfree(lpPathName);
	SetLastError(err);
	return ret;
}

static __inline DWORD GetModuleFileNameU(HMODULE hModule, char* lpFilename, DWORD nSize)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpFilename, nSize);
	ret = GetModuleFileNameW(hModule, wlpFilename, nSize);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpFilename, lpFilename, nSize)) == 0)) {
		err = GetLastError();
	}
	wfree(lpFilename);
	SetLastError(err);
	return ret;
}

static __inline DWORD GetModuleFileNameExU(HANDLE hProcess, HMODULE hModule, char* lpFilename, DWORD nSize)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(lpFilename, nSize);
	ret = GetModuleFileNameExW(hProcess, hModule, wlpFilename, nSize);
	err = GetLastError();
	if ((ret != 0)
		&& ((ret = wchar_to_utf8_no_alloc(wlpFilename, lpFilename, nSize)) == 0)) {
		err = GetLastError();
	}
	wfree(lpFilename);
	SetLastError(err);
	return ret;
}

static __inline DWORD GetFullPathNameU(const char* lpFileName, DWORD nBufferLength, char* lpBuffer, char** lpFilePart)
{
	DWORD ret = 0, err = ERROR_INVALID_DATA;
	wchar_t* wlpFilePart;
	wconvert(lpFileName);
	// coverity[returned_null]
	walloc(lpBuffer, nBufferLength);

	// lpFilePart is not supported
	if (lpFilePart != NULL) goto out;

	ret = GetFullPathNameW(wlpFileName, nBufferLength, wlpBuffer, &wlpFilePart);
	err = GetLastError();
	if ((ret != 0) && ((ret = wchar_to_utf8_no_alloc(wlpBuffer, lpBuffer, nBufferLength)) == 0)) {
		err = GetLastError();
	}

out:
	wfree(lpBuffer);
	wfree(lpFileName);
	SetLastError(err);
	return ret;
}

// needed
// The following expects PropertyBuffer to contain a single Unicode string
static __inline BOOL SetupDiGetDeviceRegistryPropertyU(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData,
	DWORD Property, PDWORD PropertyRegDataType, PBYTE PropertyBuffer, DWORD PropertyBufferSize, PDWORD RequiredSize)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	// coverity[returned_null]
	walloc(PropertyBuffer, PropertyBufferSize);

	ret = SetupDiGetDeviceRegistryPropertyW(DeviceInfoSet, DeviceInfoData, Property,
		PropertyRegDataType, (PBYTE)wPropertyBuffer, PropertyBufferSize, RequiredSize);
	err = GetLastError();
	if ((ret != 0) && (wchar_to_utf8_no_alloc(wPropertyBuffer,
		(char*)(uintptr_t)PropertyBuffer, PropertyBufferSize) == 0)) {
		err = GetLastError();
		ret = FALSE;
	}
	wfree(PropertyBuffer);
	SetLastError(err);
	return ret;
}

// keep
static __inline BOOL GetVolumeInformationU(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer,
	DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength,
	LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
	BOOL ret = FALSE;
	DWORD err = ERROR_INVALID_DATA;
	wconvert(lpRootPathName);
	// coverity[returned_null]
	walloc(lpVolumeNameBuffer, nVolumeNameSize);
	// coverity[returned_null]
	walloc(lpFileSystemNameBuffer, nFileSystemNameSize);

	ret = GetVolumeInformationW(wlpRootPathName, wlpVolumeNameBuffer, nVolumeNameSize,
		lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags,
		wlpFileSystemNameBuffer, nFileSystemNameSize);
	err = GetLastError();
	if (ret) {
		if ( ((lpVolumeNameBuffer != NULL) && (wchar_to_utf8_no_alloc(wlpVolumeNameBuffer,
			lpVolumeNameBuffer, nVolumeNameSize) == 0))
		  || ((lpFileSystemNameBuffer != NULL) && (wchar_to_utf8_no_alloc(wlpFileSystemNameBuffer,
			lpFileSystemNameBuffer, nFileSystemNameSize) == 0)) ) {
			err = GetLastError();
			ret = FALSE;
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
