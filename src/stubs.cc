#include <QtGlobal>

#include "gondar.h"

DeviceGuyList* GetDeviceList() {
  auto* list = DeviceGuyList_init();
  DeviceGuyList_append(list, 0, "stubdevice0");
  DeviceGuyList_append(list, 1, "stubdevice1");
  return list;
}

void Install(DeviceGuy* target_device, const char* image_path,
             int64_t image_size) {
  Q_UNUSED(target_device);
  Q_UNUSED(image_path);
}

BOOL IsCurrentProcessElevated() {
  return true;
}
