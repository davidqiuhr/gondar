#ifndef GONDAR_H
#define GONDAR_H

#include "deviceguy.h"
#include "shared.h"

DeviceGuyList * GetDeviceList();
void Install(DeviceGuy * target_device, const char * image_path);
BOOL IsCurrentProcessElevated();

#endif
