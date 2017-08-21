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

#ifndef SRC_DEVICE_LIST_MODEL_H_
#define SRC_DEVICE_LIST_MODEL_H_

#include <vector>

#include <QAbstractListModel>

#include "device.h"
#include "option.h"

namespace gondar {

class DeviceListModel : public QAbstractListModel {
 public:
  enum class Column {
    Id = 0,
    Name = 1,
    NumColumns = 2,
  };

  Option<DeviceGuy> deviceFromRow(const int row) const;
  bool hasDevice(const DeviceGuy& device) const;

  void addDevice(DeviceGuy&& device);
  void addDevice(const DeviceGuy& device);

  void refresh(const DeviceGuyList& updated_devices);

  // QAbstractItemModel methods
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;

 private:
  DeviceGuyList devices_;
};

}  // namespace gondar

#endif  // SRC_DEVICE_LIST_MODEL_H_
