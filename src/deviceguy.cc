#include "deviceguy.h"

void DeviceList_append(DeviceList* list, const DeviceGuy* device) {
  (*list)->emplace_back(*device);
}
