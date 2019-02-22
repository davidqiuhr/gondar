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

#if defined(Q_OS_WIN)
#include <windows.h>
#undef WIN32_NO_STATUS

#include <bcrypt.h>
#include <ntstatus.h>
#include <sal.h>
#include <stdio.h>
#include <winerror.h>
#include <winternl.h>
#endif

#include <random>

#include "log.h"

namespace gondar {

RandomManager::RandomManager() {
  // we initialize the seed
  seed = getSeed();
  gen.seed(seed);
}

#if defined(Q_OS_WIN)
unsigned int RandomManager::getSeed() {
  BYTE Buffer[4];
  DWORD BufferSize;

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

unsigned int RandomManager::getSeed() {
  // FIXME(kendall): make this work on linux
  std::random_device rd;
  return rd();
}
#endif

// the range is inclusive according to
// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
// I believe a seed is generated on each call this way, but we only call it
// twice so that should be fine.
int getRandomNum(int lower, int higher) {
  std::uniform_int_distribution<> dis(lower, higher);
  auto gen = gondar::RandomManager::getInstance().gen;
  int result = dis(gen);
  LOG_INFO << "generated " << result;
  return result;
}
}  // namespace gondar
