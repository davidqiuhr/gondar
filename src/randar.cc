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

#include <winternl.h>
#include <ntstatus.h>
#include <winerror.h>
#include <stdio.h>
#include <bcrypt.h>
#include <sal.h>
#endif

namespace gondar {

RandomManager::RandomManager() {
}

#if defined(Q_OS_WIN)
// TODO: rename getRandomByte?
unsigned int RandomManager::getRandomByte() {
  uint8_t Buffer[4];
  uint16_t BufferSize;
  //BYTE Buffer[4];
  //DWORD BufferSize;

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
  // FIXME(kendall): make this work on linux
  // TODO: move these static guys into the class
  static std::random_device rd;
  static std::mt19937 gen(rd());
  // 2**32 - 1
  //std::uniform_int_distribution<> dis(0, 4294967295);
  //unsigned int result = dis(gen);
  unsigned int result = gen();
  LOG_INFO << "generated " << result;
  return result;
}
#endif

int getRandomNum(int lower, int higher) {
  unsigned int byte = gondar::RandomManager::getInstance().getRandomByte();
  return (byte % (higher - lower)) + lower;
}

}  // namespace gondar
