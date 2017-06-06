#include <string.h>

#include "list_t.h"
#include "shared_allocator.h"
#include "tuple_t.h"
#include "monitor.h"


void initialize_list(ptr_t listPtr)
{
    list_t *list = deref_ptr(listPtr);
    if(list == NULL)
        return;
    list->size = 0;
    list->head = 0;
    list->monitor = create_monitor();
    list->condition = create_condition_variable();
}

void destroy_list(ptr_t listPtr)
{
    list_t *list = deref_ptr(listPtr);
    if(list == NULL)
        return;
    destroy_condition_variable(list->condition);
    destroy_monitor(list->monitor);
    ptr_t dataPtr = 0;
    while(list->size > 0)
        if(remove_from_list_after(listPtr, 0, &dataPtr) == 0)
        {
            set_remote_mem_ops(true);
            destroy_tuple(dataPtr);
            set_remote_mem_ops(false);
        }
}

int insert_into_list_after(ptr_t listPtr, ptr_t elementPtr, const ptr_t dataPtr)
{
    list_t *list = deref_ptr(listPtr);
    if(list == NULL)
        return -1;
    ptr_t newElementPtr = balloc(sizeof(list_element_t));
    enter_monitor(list->monitor);
    ptr_t data = clone_into_shared_memory(dataPtr);
    list_element_t *newElement = deref_ptr(newElementPtr);
    if(newElement == NULL)
        return -1;
    newElement->data = data;
    list_element_t *element = deref_ptr(elementPtr);
    list = deref_ptr(listPtr);
    if(element == NULL)
    {
        newElement->next = list->head;
        list->head = newElementPtr;
    }
    else
    {
        newElement->next = element->next;
        element->next = newElementPtr;
    }
    ++list->size;
    return 0;
}

int remove_from_list_after(ptr_t listPtr, ptr_t elementPtr, ptr_t *dataPtr)
{
    list_t *list = deref_ptr(listPtr);
    if(list == NULL)
        return -1;
    if(list->size == 0)
        return -1;
    list_element_t *element = deref_ptr(elementPtr);
    ptr_t oldElement = 0;
    if(element == NULL)
    {
        list_element_t *head = deref_ptr(list->head);
        *dataPtr = head->data;
        oldElement = list->head;
        list->head = head->next;
    }
    else
    {
        if(is_ptr_null(element->next))
            return -1;
        list_element_t *next = deref_ptr(element->next);
        *dataPtr = next->data;
        oldElement = element->next;
        element->next = next->next;
    }
    bfree(oldElement);
    --list->size;
    return 0;
}
