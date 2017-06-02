#ifndef UXP1A_LIST_T_H
#define UXP1A_LIST_T_H

#include <stdint.h>
#include <stdlib.h>
#include "utils.h"


typedef struct list_element_t list_element_t;
struct list_element_t
{
    ptr_t data;
    ptr_t next;
};

typedef struct list_t
{
    size_t size;
    ptr_t head;
    ptr_t monitor;
    ptr_t condition;
} list_t;

void initialize_list(ptr_t listPtr);
void destroy_list(ptr_t listPtr);
int insert_into_list_after(ptr_t listPtr, ptr_t elementPtr, const ptr_t dataPtr);
int remove_from_list_after(ptr_t listPtr, ptr_t elementPtr, ptr_t *dataPtr);


#endif //UXP1A_LIST_T_H
