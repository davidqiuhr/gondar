#ifndef GONDAR_H
#define GONDAR_H

#include "deviceguy.h"
#include "shared.h"

#ifdef __cplusplus
extern "C" {
#endif

DeviceGuyList* GetDeviceList();

// Returns true on success
BOOL Install(DeviceGuy* target_device,
             const char* image_path,
             int64_t image_size);
BOOL IsCurrentProcessElevated();

#ifdef __cplusplus
}
#endif

#endif
