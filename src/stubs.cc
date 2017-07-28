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

#include <QtGlobal>

#include "gondar.h"

DeviceGuyList* GetDeviceList() {
  auto* list = DeviceGuyList_init();
  DeviceGuyList_append(list, 0, "stubdevice0");
  DeviceGuyList_append(list, 1, "stubdevice1");
  return list;
}

BOOL Install(DeviceGuy* target_device,
             const char* image_path,
             int64_t image_size) {
  Q_UNUSED(target_device);
  Q_UNUSED(image_path);
  Q_UNUSED(image_size);
  return true;
}

BOOL IsCurrentProcessElevated() {
  return true;
}
