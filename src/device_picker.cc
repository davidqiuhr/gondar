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

#include "device_picker.h"

#include <stdexcept>

#include "device.h"
#include "gondar.h"
#include "log.h"
#include "util.h"

namespace gondar {

DevicePicker::DevicePicker() {
  layout_.addWidget(&list_view_);
  layout_.setContentsMargins(0, 0, 0, 0);
  setLayout(&layout_);

  list_view_.setEditTriggers(QAbstractItemView::NoEditTriggers);
  list_view_.setModel(&model_);

  connect(list_view_.selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &DevicePicker::selectionChanged);
}

bool DevicePicker::hasSelection() const {
  return list_view_.selectionModel()->hasSelection();
}

bool DevicePicker::isEmpty() const {
  return model_.rowCount() == 0;
}

Option<DeviceGuy> DevicePicker::selection() const {
  const auto selection = list_view_.selectionModel()->selection();
  const auto indices = selection.indexes();

  if (indices.isEmpty()) {
    return nullopt;
  } else if (indices.size() > 1) {
    throw std::runtime_error("multiple selections");
  }

  return model_.deviceFromRow(indices[0].row());
}

void DevicePicker::refresh() {
  refresh(GetDeviceList());
}

void DevicePicker::refresh(const DeviceGuyList& devices) {
  model_.refresh(devices);
}

const QListView& DevicePicker::listView() const {
  return list_view_;
}

}  // namespace gondar
