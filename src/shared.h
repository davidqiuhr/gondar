// Copyright 2017 Neverware
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

/*
At the moment this is not necessary; these defines could just be moved to
gondar.h

However, I've included it in anticipation of utility function shared between
files
*/
#ifndef SHARED_H
#define SHARED_H

#include <algorithm>
#include <cstdint>

#define TRUE 1
#define FALSE 0
#define BOOL int32_t

#define safe_sprintf(dst, count, ...)   \
  do {                                  \
    _snprintf(dst, count, __VA_ARGS__); \
    (dst)[(count)-1] = 0;               \
  } while (0)
#define static_sprintf(dst, ...) safe_sprintf(dst, sizeof(dst), __VA_ARGS__)
#define safe_strlen(str) ((((char*)str) == NULL) ? 0 : strlen(str))
#define safe_strcmp(str1, str2) \
  strcmp(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2))
#define safe_stricmp(str1, str2)               \
  _stricmp(((str1 == NULL) ? "<NULL>" : str1), \
           ((str2 == NULL) ? "<NULL>" : str2))
#define safe_strcpy(dst, dst_max, src) \
  safe_strcp(dst, dst_max, src, safe_strlen(src) + 1)
#define safe_strcp(dst, dst_max, src, count)        \
  do {                                              \
    memcpy(dst, src, safe_min(count, dst_max));     \
    ((char*)dst)[safe_min(count, dst_max) - 1] = 0; \
  } while (0)
#define safe_strstr(str1, str2) \
  strstr(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2))
#define safe_min(a, b) std::min((size_t)(a), (size_t)(b))
#define safe_strcat(dst, dst_max, src) \
  safe_strncat(dst, dst_max, src, safe_strlen(src) + 1)
#define safe_strncat(dst, dst_max, src, count) \
  strncat(dst, src, safe_min(count, dst_max - safe_strlen(dst) - 1))

#define safe_strdup _strdup

#endif /* SHARED_H */
