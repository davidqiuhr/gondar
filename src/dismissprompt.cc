// Copyright 2019 Neverware
//
// The source code in this file was derived from:
//
//     Rufus: The Reliable USB Formatting Utility
//     Copyright © 2011-2016 Pete Batard <pete@akeo.ie>
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

#include "dismissprompt.h"

#include <commdlg.h>
#include <richedit.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windowsx.h>

#include <QLocale>

#include "log.h"
#include "shared.h"

char* GetCurrentMUI(void);

void ClrFormatPromptHook(void);

static HWINEVENTHOOK fp_weh = NULL;
static const char *fp_title_str = "Microsoft Windows",
                  *fp_button_str = "Format disk";

// this func from stdfn, but reimplemented using Qt
char* GetCurrentMUI(void) {
  static char mui_str[LOCALE_NAME_MAX_LENGTH];
  QLocale locale;
  QByteArray localeBytes = locale.uiLanguages()[0].toUtf8();
  const char* locale_c_str = localeBytes.data();
  LOG_INFO << "searching for format prompts in language (utf8): "
           << locale_c_str;
  static_strcpy(mui_str, locale_c_str);
  return mui_str;
}

/*
 * The following function calls are used to automatically detect and close the
 * native
 * Windows format prompt "You must format the disk in drive X:". To do that, we
 * use an
 * event hook that gets triggered whenever a window is placed in the foreground.
 * In that hook, we look for a dialog that has style WS_POPUPWINDOW and has the
 * relevant
 * title. However, because the title in itself is too generic (the expectation
 * is that
 * it will be "Microsoft Windows") we also enumerate all the child controls from
 * that
 * prompt, using another callback, until we find one that contains the text we
 * expect
 * for the "Format disk" button.
 * Oh, and since all of these strings are localized, we must first pick them up
 * from
 * the relevant mui (something like "C:\Windows\System32\en-GB\shell32.dll.mui")
 */
static BOOL CALLBACK FormatPromptCallback(HWND hWnd, LPARAM lParam) {
  char str[128];
  BOOL* found = (BOOL*)lParam;

  if (GetWindowTextU(hWnd, str, sizeof(str)) == 0)
    return TRUE;
  if (safe_strcmp(str, fp_button_str) == 0)
    *found = TRUE;
  return TRUE;
}

static void CALLBACK FormatPromptHook(HWINEVENTHOOK,
                                      DWORD Event,
                                      HWND hWnd,
                                      LONG,
                                      LONG,
                                      DWORD,
                                      DWORD) {
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
          printf("Closed Windows format prompt");
        }
      }
    }
  }
}

BOOL SetFormatPromptHook(void) {
  HMODULE mui_lib;
  char mui_path[MAX_PATH];
  static char title_str[128], button_str[128];
  char system_dir[MAX_PATH];

  if (fp_weh != NULL)
    return TRUE;  // No need to set again if active

  // Fetch the localized strings in the relevant
  static_sprintf(mui_path, "%s\\%s\\shell32.dll.mui", system_dir,
                 GetCurrentMUI());
  mui_lib = LoadLibraryU(mui_path);
  if (mui_lib != NULL) {
    // 4097 = "You need to format the disk in drive %c: before you can use it."
    // (dialog text)
    // 4125 = "Microsoft Windows" (dialog title)
    // 4126 = "Format disk" (button)
    if (LoadStringU(mui_lib, 4125, title_str, sizeof(title_str)) > 0)
      fp_title_str = title_str;
    else
      printf(
          "Warning: Could not locate localized format prompt title string in "
          "'%s'",
          mui_path);
    if (LoadStringU(mui_lib, 4126, button_str, sizeof(button_str)) > 0)
      fp_button_str = button_str;
    else
      printf(
          "Warning: Could not locate localized format prompt button string in "
          "'%s'",
          mui_path);
    FreeLibrary(mui_lib);
  }

  fp_weh = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                           NULL, FormatPromptHook, 0, 0,
                           WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  return (fp_weh != NULL);
}

void ClrFormatPromptHook(void) {
  UnhookWinEvent(fp_weh);
  fp_weh = NULL;
}
