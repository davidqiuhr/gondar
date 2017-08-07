// Copyright 2017 Neverware
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

#include "log.h"
#include "shared.h"

DeviceGuy* DeviceGuy_init(uint32_t device_num, const std::string& name) {
  DeviceGuy* self = new DeviceGuy();
  self->device_num = device_num;
  self->name = name;
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
                          const std::string& name) {
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
    LOG_INFO << "device_num=" << itr->device_num << ", name=" << itr->name;
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
  LOG_ERROR << "device at " << index << " not found";
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
