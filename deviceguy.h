/*
A linkedlist of device info passed between the C and C++ layer
*/
#ifndef DEVICEGUY_H
#define DEVICEGUY_H

#ifndef NULL
#define NULL 0
#endif

#include <inttypes.h>

//TODO(kendall): rename to something more thoughtful
typedef struct DeviceGuy {
    uint32_t device_num;
    const char * name;
    struct DeviceGuy * next;
    struct DeviceGuy * prev;
} DeviceGuy;

typedef struct DeviceGuyList {
    DeviceGuy * head;
    DeviceGuy * tail;
} DeviceGuyList;

DeviceGuy * DeviceGuy_init();
DeviceGuyList * DeviceGuyList_init();
void DeviceGuyList_append(DeviceGuyList * self, uint32_t index, char * name);
void DeviceGuyList_print(DeviceGuyList * self); 
DeviceGuy * DeviceGuyList_getByIndex(DeviceGuyList * self, uint32_t index); 
uint32_t DeviceGuyList_length(DeviceGuyList * self); 
void DeviceGuyList_free(DeviceGuyList * self); 

#endif /* DEVICEGUY_H */
