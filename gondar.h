#ifndef GONDAR_H
#define GONDAR_H

#include "deviceguy.h"
#include "shared.h"

DeviceGuyList * GetDeviceList();
void Install(DeviceGuy * target_device, char * image_path);

//FIXME(kendall): temp?
BOOL IsCurrentProcessElevated();

#endif
