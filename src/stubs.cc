#include <QtGlobal>

#include "gondar.h"

void GetDeviceList(DeviceList* list) {
  const DeviceGuy device0 = {0, "stubdevice0"};
  const DeviceGuy device1 = {1, "stubdevice1"};
  DeviceList_append(list, &device0);
  DeviceList_append(list, &device1);
}

void Install(DeviceGuy* target_device, const char* image_path) {
  Q_UNUSED(target_device);
  Q_UNUSED(image_path);
}

BOOL IsCurrentProcessElevated() {
  return true;
}
