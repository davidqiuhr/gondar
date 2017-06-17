#ifndef DEVICE_H
#define DEVICE_H

#include <list>

struct DeviceList {
  std::list<DeviceGuy> list;

  decltype(list) * operator->() { return &list; }

  decltype(list) & operator*() { return list; }

  DeviceGuy* operator[](const unsigned index) {
    if (index < list.size()) {
      return &(*std::next(list.begin(), index));
    } else {
      return nullptr;
    }
  }
};

#endif
