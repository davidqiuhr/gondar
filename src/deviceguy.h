/*
A linkedlist of device info passed between the C and C++ layer
*/
#ifndef DEVICEGUY_H
#define DEVICEGUY_H

#include <inttypes.h>

#ifdef __cplusplus
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
#endif

#endif /* DEVICEGUY_H */
