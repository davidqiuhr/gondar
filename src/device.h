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

/*
A linkedlist of device info passed between the C and C++ layer
*/
#ifndef DEVICE_H
#define DEVICE_H

#include <cstdint>
#include <string>

// TODO(kendall): rename to something more thoughtful
typedef struct DeviceGuy {
  uint32_t device_num = 0;
  std::string name;
  struct DeviceGuy* next = nullptr;
  struct DeviceGuy* prev = nullptr;
} DeviceGuy;

typedef struct DeviceGuyList {
  DeviceGuy* head = nullptr;
  DeviceGuy* tail = nullptr;
} DeviceGuyList;

DeviceGuy* DeviceGuy_init(uint32_t device_num, const std::string& name);
void DeviceGuy_copy(const DeviceGuy* src, DeviceGuy* dst);
DeviceGuyList* DeviceGuyList_init();
void DeviceGuyList_append(DeviceGuyList* self,
                          uint32_t index,
                          const std::string& name);
void DeviceGuyList_print(DeviceGuyList* self);
DeviceGuy* DeviceGuyList_getByIndex(DeviceGuyList* self, uint32_t index);
uint32_t DeviceGuyList_length(DeviceGuyList* self);
void DeviceGuyList_free(DeviceGuyList* self);

#endif /* DEVICE_H */
