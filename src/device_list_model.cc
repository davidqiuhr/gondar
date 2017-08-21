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

#include "device_list_model.h"
#include "log.h"
#include "util.h"

namespace gondar {

Option<DeviceGuy> DeviceListModel::deviceFromRow(const int row) const {
  if (row >= 0 && ((unsigned int)row < devices_.size())) {
    return devices_.at(row);
  }
  return nullopt;
}

bool DeviceListModel::hasDevice(const DeviceGuy& device) const {
  return contains(devices_, device);
}

void DeviceListModel::addDevice(DeviceGuy&& device) {
  const int row = devices_.size();
  beginInsertRows(QModelIndex(), row, row);
  devices_.emplace_back(device);
  endInsertRows();
}

void DeviceListModel::addDevice(const DeviceGuy& device) {
  addDevice(DeviceGuy(device));
}

void DeviceListModel::refresh(const DeviceGuyList& updated_devices) {
  LOG_INFO << "DeviceListModel: starting update";

  // Remove rows for any devices that are no longer present
  std::size_t row = 0;
  while (row < devices_.size()) {
    const auto& old_device = devices_[row];

    if (contains(updated_devices, old_device)) {
      row++;
    } else {
      LOG_INFO << "DeviceListModel: removing " << old_device;
      beginRemoveRows(QModelIndex(), row, row);
      devices_.erase(devices_.begin() + row);
      endRemoveRows();
    }
  }

  // Add any new devices to the end of the model (to keep order
  // stable)
  for (const auto& device : updated_devices) {
    if (!hasDevice(device)) {
      LOG_INFO << "DeviceListModel: adding " << device;
      addDevice(device);
    }
  }
}

int DeviceListModel::columnCount(const QModelIndex&) const {
  return static_cast<int>(Column::NumColumns);
}

QVariant DeviceListModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole && index.isValid()) {
    const auto& device = devices_.at(index.row());
    if (index.column() == static_cast<int>(Column::Id)) {
      return device.device_num;
    } else if (index.column() == static_cast<int>(Column::Name)) {
      return QString::fromStdString(device.name);
    }
  }

  return QVariant();
}

QVariant DeviceListModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const {
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
    return QVariant();
  } else if (section == static_cast<int>(Column::Id)) {
    return "Id";
  } else if (section == static_cast<int>(Column::Name)) {
    return "Name";
  } else {
    return QVariant();
  }
}

int DeviceListModel::rowCount(const QModelIndex&) const {
  return devices_.size();
}

}  // namespace gondar
