
#include "randar.h"

#include <random>

#if defined(Q_OS_WIN)
#include <windows.h>
#undef WIN32_NO_STATUS

#include <winternl.h>
#include <ntstatus.h>
#include <winerror.h>
#include <stdio.h>
#include <bcrypt.h>
#include <sal.h>
#endif

#include "log.h"

namespace gondar {

RandomManager::RandomManager() {
  // we initialize the seed
  seed = getSeed();
  gen.seed(seed);
}

#if defined(Q_OS_WIN)
unsigned int RandomManager::getSeed() {
    BYTE    Buffer[4];
    DWORD   BufferSize;
    
    BufferSize = sizeof (Buffer);
    memset (Buffer, 0, BufferSize);
    
    //
    // Fill the buffer with random bytes
    //

    BCryptGenRandom(NULL,  // Alg Handle pointer; NUll is passed as BCRYPT_USE_SYSTEM_PREFERRED_RNG flag is used
                    Buffer,                     // Address of the buffer that recieves the random number(s)
                    BufferSize,                 // Size of the buffer in bytes
                    BCRYPT_USE_SYSTEM_PREFERRED_RNG); // Flags                  
    int ret = (Buffer[3] << 24) | (Buffer[2] << 16) | (Buffer[1] << 8) | (Buffer[0]);
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
int RandomManager::getRandomNum(int lower, int higher) {
  std::uniform_int_distribution<> dis(lower, higher);
  int result = dis(gen);
  LOG_INFO << "generated " << result;
  return result;
}
} // namespace gondar
