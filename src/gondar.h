#ifndef GONDAR_H
#define GONDAR_H

#include "deviceguy.h"
#include "shared.h"

#ifdef __cplusplus
extern "C" {
#endif

DeviceGuyList* GetDeviceList();
void Install(DeviceGuy* target_device, const char* image_path);
BOOL IsCurrentProcessElevated();

#ifdef __cplusplus
}
#endif

#endif
