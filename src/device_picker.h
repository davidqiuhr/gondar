// Copyright 2017 Neverware

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
