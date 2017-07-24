/*
A linkedlist of device info passed between the C and C++ layer
*/
#ifndef DEVICEGUY_H
#define DEVICEGUY_H

#include <inttypes.h>

#ifdef __cplusplus
#include <ostream>
#include <string>
#include <vector>

#include "log.h"
extern "C" {
#endif

#define MAX_NAME_LENGTH 64

// TODO(kendall): rename to something more thoughtful
typedef struct DeviceGuy {
  uint32_t device_num;
  char name[MAX_NAME_LENGTH];
  struct DeviceGuy* next;
  struct DeviceGuy* prev;
} DeviceGuy;

typedef struct DeviceGuyList {
  DeviceGuy* head;
  DeviceGuy* tail;
} DeviceGuyList;

DeviceGuy* DeviceGuy_init(uint32_t device_num, const char* name);
void DeviceGuy_copy(const DeviceGuy* src, DeviceGuy* dst);
DeviceGuyList* DeviceGuyList_init();
void DeviceGuyList_append(DeviceGuyList* self,
                          uint32_t index,
                          const char* name);
void DeviceGuyList_print(DeviceGuyList* self);
DeviceGuy* DeviceGuyList_getByIndex(DeviceGuyList* self, uint32_t index);
uint32_t DeviceGuyList_length(DeviceGuyList* self);
void DeviceGuyList_free(DeviceGuyList* self);

#ifdef __cplusplus
}
namespace gondar {

class Device {
 public:
  class Id {
   public:
    explicit Id(uint32_t value) : value_(value) {}

    uint32_t value() const { return value_; }

    bool operator==(const Id& other) const;
    bool operator<(const Id& other) const;

   private:
    uint32_t value_;
  };

  static std::vector<Device> sortedDevices();

  Device(const std::string& name, const Id id) : name_(name), id_(id) {}

  const std::string& name() const { return name_; }

  Id id() const { return id_; }

  bool operator==(const Device& other) const;
  bool operator<(const Device& other) const;
  std::ostream& operator<<(std::ostream& stream) const;

 private:
  std::string name_;
  Id id_;
};

}  // namespace gondar

//namespace plog {
plog::Record& operator<<(plog::Record& record, const gondar::Device& device);
//}  // namespace plog

#endif

#endif /* DEVICEGUY_H */
