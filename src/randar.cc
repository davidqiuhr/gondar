// Copyright 2019 Neverware
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

#include "randar.h"

#include <random>

#include "log.h"

#if defined(Q_OS_WIN)
#include <windows.h>

#include <bcrypt.h>
#include <ntstatus.h>
#include <sal.h>
#include <stdio.h>
#include <winerror.h>
#include <winternl.h>
#endif

namespace gondar {

RandomManager::RandomManager() {
  // only used in linux implementation for now
  gen.seed(rd());
}

#if defined(Q_OS_WIN)
unsigned int RandomManager::getRandomByte() {
  uint8_t Buffer[4];
  uint16_t BufferSize;

  BufferSize = sizeof(Buffer);
  memset(Buffer, 0, BufferSize);

  // fills buffer with random bytes
  BCryptGenRandom(NULL, Buffer, BufferSize,
                  BCRYPT_USE_SYSTEM_PREFERRED_RNG);  // Flags
  int ret =
      (Buffer[3] << 24) | (Buffer[2] << 16) | (Buffer[1] << 8) | (Buffer[0]);
  return ret;
}
#else

unsigned int RandomManager::getRandomByte() {
  return gen();
}
#endif

int getRandomNum(int lower, int higher) {
  unsigned int byte = gondar::RandomManager::getInstance().getRandomByte();
  return (byte % (higher - lower)) + lower;
}

}  // namespace gondar
