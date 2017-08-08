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

#ifndef SRC_DEVICE_PICKER_H_
#define SRC_DEVICE_PICKER_H_

#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

namespace gondar {

class Device;

class DevicePicker : public QWidget {
 public:
  DevicePicker();

  bool hasSelection();

  void refresh();

 private:
  Device deviceFromRow(int row) const;

  bool containsDevice(const Device& device) const;

  QItemSelectionModel* selectionModel();

  void selectionChanged(const QItemSelection& selected,
                        const QItemSelection& deselected);

  void ensureSomethingSelected();

  QVBoxLayout layout_;
  QListView list_view_;
  QStandardItemModel model_;
};

}  // namespace gondar

#endif  // SRC_DEVICE_PICKER_H_
