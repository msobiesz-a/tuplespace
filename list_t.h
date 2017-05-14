#ifndef UXP1A_LIST_T_H
#define UXP1A_LIST_T_H

#include <stdlib.h>


typedef struct list_element_t list_element_t;
struct list_element_t
{
    size_t data;
    size_t next;
};

typedef struct list_t
{
    size_t size;
    size_t head;
} list_t;

void initialize_list(size_t listPtr);
void destroy_list(size_t listPtr);
int insert_into_list_after(size_t listPtr, size_t elementPtr, const size_t dataPtr);
int remove_from_list_after(size_t listPtr, size_t elementPtr, size_t *dataPtr);


#endif //UXP1A_LIST_T_H
