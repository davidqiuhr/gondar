
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "deviceguy.h"

#include "shared.h"

const uint32_t MAX_NAME_LENGTH = 64;

DeviceGuy * DeviceGuy_init(uint32_t device_num, char * name) {
    DeviceGuy * self = malloc(sizeof(DeviceGuy));
    memset(self, 0, sizeof(DeviceGuy));
    self->device_num = device_num;
    // the name that came in is just a temporary buffer; we'll need to strcpy
    self->name = malloc(MAX_NAME_LENGTH); 
    memset(self->name, 0, MAX_NAME_LENGTH);
    // maybe my strcpy is bad?
    strcpy(self->name, name);
    printf("kendall: new deviceguy; device_num=%d, name=%s, next=%d, prev=%d\n",
            self->device_num, self->name, self->next, self->prev);
    return self;
}

DeviceGuyList * DeviceGuyList_init() {
    DeviceGuyList * self = malloc(sizeof(DeviceGuyList));
    memset(self, 0, sizeof(DeviceGuyList));
    self->head = NULL;
    self->tail = NULL;
    return self;
}

void DeviceGuyList_append(DeviceGuyList * self, uint32_t device_num, char * name) {
    printf("kendall: in append with self=%d, device_num=%d, name=%s\n", self, device_num, name);
    DeviceGuy * newguy = DeviceGuy_init(device_num, name);
    if (self->head == NULL) {
        //FIXME(kendall): when i set self->head to newguy, everything breaks 
        self->head = newguy;
        self->tail = newguy;
    }
    else {
        self->tail->next = newguy;
        newguy->prev = self->tail;
        self->tail = newguy;
    }
}

void DeviceGuyList_print(DeviceGuyList * self) {
    DeviceGuy * itr = self->head;
    while (itr != NULL) {
        printf("device_num=%d,name=%s\n", itr->device_num, itr->name);
        itr = itr->next;
    }
}

DeviceGuy * DeviceGuyList_getByIndex(DeviceGuyList * self, uint32_t index) {
    DeviceGuy * itr = self->head;
    while (self->head != NULL) {
        if (itr->device_num == index) {
            return itr;
        }
        itr = itr->next;
    }
    printf("ERROR: Device index not found!\nHave a nice day!\n");
    return NULL;
}

uint32_t DeviceGuyList_length(DeviceGuyList * self) {
    uint32_t length = 0;
    DeviceGuy * itr = self->head;
    while (itr != NULL) {
       itr = itr->next;
       length++; 
    }
    return length;
}

void DeviceGuyList_free(DeviceGuyList * self) {
    DeviceGuy * itr = self->head;
    DeviceGuy * next;
    while (itr != NULL) {
        next = itr->next; 
        free(itr->name);
        free(itr);
        itr = next;
    }
    free(self);
}
