#include <algorithm>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deviceguy.h"
#include "gondar.h"

#include "shared.h"

DeviceGuy* DeviceGuy_init(uint32_t device_num, const char* name) {
  DeviceGuy* self = new DeviceGuy();
  memset(self, 0, sizeof(DeviceGuy));
  self->device_num = device_num;
  // the name that came in is just a temporary buffer; we'll need to strcpy
  strncpy(self->name, name, MAX_NAME_LENGTH);
  self->name[MAX_NAME_LENGTH - 1] = '\0';
  printf("kendall: new deviceguy; device_num=%d, name=%s, next=%p, prev=%p\n",
         self->device_num, self->name, self->next, self->prev);
  return self;
}

void DeviceGuy_copy(const DeviceGuy* src, DeviceGuy* dst) {
  *dst = *src;
  // we have no interest in copying the list-ness of this structure
  dst->next = NULL;
  dst->prev = NULL;
}
DeviceGuyList* DeviceGuyList_init() {
  DeviceGuyList* self = new DeviceGuyList();
  memset(self, 0, sizeof(DeviceGuyList));
  self->head = NULL;
  self->tail = NULL;
  return self;
}

void DeviceGuyList_append(DeviceGuyList* self,
                          uint32_t device_num,
                          const char* name) {
  printf("kendall: in append with self=%p, device_num=%d, name=%s\n", self,
         device_num, name);
  DeviceGuy* newguy = DeviceGuy_init(device_num, name);
  if (self->head == NULL) {
    self->head = newguy;
    self->tail = newguy;
  } else {
    self->tail->next = newguy;
    newguy->prev = self->tail;
    self->tail = newguy;
  }
}

void DeviceGuyList_print(DeviceGuyList* self) {
  DeviceGuy* itr = self->head;
  while (itr != NULL) {
    printf("device_num=%d,name=%s\n", itr->device_num, itr->name);
    itr = itr->next;
  }
}

DeviceGuy* DeviceGuyList_getByIndex(DeviceGuyList* self, uint32_t index) {
  DeviceGuy* itr = self->head;
  while (self->head != NULL) {
    if (itr->device_num == index) {
      return itr;
    }
    itr = itr->next;
  }
  printf("ERROR: Device index not found!\nHave a nice day!\n");
  return NULL;
}

uint32_t DeviceGuyList_length(DeviceGuyList* self) {
  uint32_t length = 0;
  DeviceGuy* itr = self->head;
  while (itr != NULL) {
    itr = itr->next;
    length++;
  }
  return length;
}

void DeviceGuyList_free(DeviceGuyList* self) {
  DeviceGuy* itr = self->head;
  DeviceGuy* next;
  while (itr != NULL) {
    next = itr->next;
    delete itr;
    itr = next;
  }
  delete self;
}

namespace gondar {

bool Device::Id::operator==(const Id& other) const {
  return value() == other.value();
}

bool Device::Id::operator<(const Id& other) const {
  return value() < other.value();
}

std::vector<Device> Device::sortedDevices() {
  std::vector<Device> result;

  auto* devices = GetDeviceList();
  for (DeviceGuy* item = devices->head; item; item = item->next) {
    result.emplace_back(Device(item->name, Device::Id(item->device_num)));
  }
  DeviceGuyList_free(devices);

  std::sort(result.begin(), result.end());

  for (const auto& d : result) {
    LOG_ERROR << d;
  }

  return result;
}

bool Device::operator==(const Device& other) const {
  return name() == other.name() && id() == other.id();
}

bool Device::operator<(const Device& other) const {
  const auto n = name().compare(other.name());
  if (n == 0) {
    return id() < other.id();
  } else {
    return n;
  }
}

}  // namespace gondar

//namespace plog {
Record& operator<<(Record& record, const gondar::Device& device) {
  return record << "Device(\"" << device.name() << "\", " << device.id().value()
                << ")";
}
//}  // namespace plog
