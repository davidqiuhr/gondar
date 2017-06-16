
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "deviceguy.h"

#include "shared.h"

DeviceGuy* DeviceGuy_init(uint32_t device_num, const char* name) {
  DeviceGuy* self = new DeviceGuy();
  memset(self, 0, sizeof(DeviceGuy));
  self->device_num = device_num;
  // the name that came in is just a temporary buffer; we'll need to strcpy
  strncpy(self->name, name, MAX_NAME_LENGTH);
  self->name[MAX_NAME_LENGTH - 1] = '\0';
  printf("kendall: new deviceguy; device_num=%d, name=%s, next=%p, prev=%p\n",
         self->device_num, self->name, self->next, self->prev);
  return self;
}

void DeviceGuy_copy(const DeviceGuy* src, DeviceGuy* dst) {
  *dst = *src;
  // we have no interest in copying the list-ness of this structure
  dst->next = NULL;
  dst->prev = NULL;
}
DeviceGuyList* DeviceGuyList_init() {
  DeviceGuyList* self = new DeviceGuyList();
  memset(self, 0, sizeof(DeviceGuyList));
  self->head = NULL;
  self->tail = NULL;
  return self;
}

void DeviceGuyList_append(DeviceGuyList* self,
                          uint32_t device_num,
                          const char* name) {
  printf("kendall: in append with self=%p, device_num=%d, name=%s\n", self,
         device_num, name);
  DeviceGuy* newguy = DeviceGuy_init(device_num, name);
  if (self->head == NULL) {
    self->head = newguy;
    self->tail = newguy;
  } else {
    self->tail->next = newguy;
    newguy->prev = self->tail;
    self->tail = newguy;
  }
}

void DeviceGuyList_print(DeviceGuyList* self) {
  DeviceGuy* itr = self->head;
  while (itr != NULL) {
    printf("device_num=%d,name=%s\n", itr->device_num, itr->name);
    itr = itr->next;
  }
}

DeviceGuy* DeviceGuyList_getByIndex(DeviceGuyList* self, uint32_t index) {
  DeviceGuy* itr = self->head;
  while (self->head != NULL) {
    if (itr->device_num == index) {
      return itr;
    }
    itr = itr->next;
  }
  printf("ERROR: Device index not found!\nHave a nice day!\n");
  return NULL;
}

uint32_t DeviceGuyList_length(DeviceGuyList* self) {
  uint32_t length = 0;
  DeviceGuy* itr = self->head;
  while (itr != NULL) {
    itr = itr->next;
    length++;
  }
  return length;
}

void DeviceGuyList_free(DeviceGuyList* self) {
  DeviceGuy* itr = self->head;
  DeviceGuy* next;
  while (itr != NULL) {
    next = itr->next;
    delete itr;
    itr = next;
  }
  delete self;
}
