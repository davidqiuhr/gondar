/*
A linkedlist of device info passed between the C and C++ layer
*/
#ifndef DEVICEGUY_H
#define DEVICEGUY_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NAME_LENGTH 64

// TODO(kendall): rename to something more thoughtful
typedef struct DeviceGuy {
  uint32_t device_num;
  char name[MAX_NAME_LENGTH];
} DeviceGuy;

typedef struct DeviceList DeviceList;

void DeviceList_append(DeviceList* list, const DeviceGuy* device);

void GetDeviceList(DeviceList* list);

#ifdef __cplusplus
}

#include "device.h"
#endif

#endif /* DEVICEGUY_H */
