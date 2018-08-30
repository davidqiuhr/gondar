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

#include "util.h"

#include <QFile>
#include <QJsonDocument>
#include <QNetworkReply>

#include "config.h"
#include "log.h"

namespace gondar {

QString readUtf8File(const QString& filepath) {
  LOG_INFO << "opening " << filepath << " for reading";
  QFile file(filepath);
  if (!file.open(QFile::ReadOnly)) {
    throw std::runtime_error("error opening file for reading: " +
                             filepath.toStdString());
  }
  return QString::fromUtf8(file.readAll());
}

// helper function to determine if this build is a chromeover build
bool isChromeover() {
#ifdef CHROMEOVER
  return true;
#else
  return false;
#endif
}

bool isRelease() {
#ifdef RELEASE
  return true;
#else
  return false;
#endif
}

QString getDomain() {
  if (gondar::isRelease()) {
    LOG_INFO << "Using release licensing endpoint";
    return QString("neverware.com");
  } else {
    LOG_INFO << "Using dev licensing endpoint";
    return QString("grv.neverware.com");
  }
}

QString getGondarVersion() {
  return QString("");
}

QJsonObject jsonFromReply(QNetworkReply* reply) {
  QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
  return jsonDoc.object();
}

// the number of bytes in a gigabyte (2**30)
uint64_t getGigabyte() {
  return 1073741824LL;
}

/*
 * The following function calls are used to automatically detect and close the native
 * Windows format prompt "You must format the disk in drive X:". To do that, we use an
 * event hook that gets triggered whenever a window is placed in the foreground.
 * In that hook, we look for a dialog that has style WS_POPUPWINDOW and has the relevant
 * title. However, because the title in itself is too generic (the expectation is that
 * it will be "Microsoft Windows") we also enumerate all the child controls from that
 * prompt, using another callback, until we find one that contains the text we expect
 * for the "Format disk" button.
 * Oh, and since all of these strings are localized, we must first pick them up from
 * the relevant mui (something like "C:\Windows\System32\en-GB\shell32.dll.mui")
 */
static BOOL CALLBACK FormatPromptCallback(HWND hWnd, LPARAM lParam)
{
	char str[128];
	BOOL *found = (BOOL*)lParam;

	if (GetWindowTextU(hWnd, str, sizeof(str)) == 0)
		return TRUE;
	if (safe_strcmp(str, fp_button_str) == 0)
		*found = TRUE;
	return TRUE;
}

static void CALLBACK FormatPromptHook(HWINEVENTHOOK hWinEventHook, DWORD Event, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	char str[128];
	BOOL found;

	if (Event == EVENT_SYSTEM_FOREGROUND) {
		if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUPWINDOW) {
			str[0] = 0;
			GetWindowTextU(hWnd, str, sizeof(str));
			if (safe_strcmp(str, fp_title_str) == 0) {
				found = FALSE;
				EnumChildWindows(hWnd, FormatPromptCallback, (LPARAM)&found);
				if (found) {
					SendMessage(hWnd, WM_COMMAND, (WPARAM)IDCANCEL, (LPARAM)0);
					uprintf("Closed Windows format prompt");
				}
			}
		}
	}
}

BOOL SetFormatPromptHook(void)
{
	HMODULE mui_lib;
	char mui_path[MAX_PATH];
	static char title_str[128], button_str[128];

	if (fp_weh != NULL)
		return TRUE;	// No need to set again if active

						// Fetch the localized strings in the relevant
	static_sprintf(mui_path, "%s\\%s\\shell32.dll.mui", system_dir, GetCurrentMUI());
	mui_lib = LoadLibraryU(mui_path);
	if (mui_lib != NULL) {
		// 4097 = "You need to format the disk in drive %c: before you can use it." (dialog text)
		// 4125 = "Microsoft Windows" (dialog title)
		// 4126 = "Format disk" (button)
		if (LoadStringU(mui_lib, 4125, title_str, sizeof(title_str)) > 0)
			fp_title_str = title_str;
		else
			uprintf("Warning: Could not locate localized format prompt title string in '%s': %s", mui_path, WindowsErrorString());
		if (LoadStringU(mui_lib, 4126, button_str, sizeof(button_str)) > 0)
			fp_button_str = button_str;
		else
			uprintf("Warning: Could not locate localized format prompt button string in '%s': %s", mui_path, WindowsErrorString());
		FreeLibrary(mui_lib);
	}

	fp_weh = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
		FormatPromptHook, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	return (fp_weh != NULL);
}

void ClrFormatPromptHook(void) {
	UnhookWinEvent(fp_weh);
	fp_weh = NULL;
}

}  // namespace gondar
