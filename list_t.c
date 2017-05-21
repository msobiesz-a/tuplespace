#include <string.h>

#include "list_t.h"
#include "shared_allocator.h"
#include "tuple_t.h"
#include "monitor.h"


void initialize_list(size_t listPtr)
{
    list_t *list = dereference_pointer(listPtr);
    if(list == NULL)
        return;
    list->size = 0;
    list->head = 0;
    list->monitor = create_monitor();
    list->condition = create_condition_variable();
}

void destroy_list(size_t listPtr)
{
    list_t *list = dereference_pointer(listPtr);
    if(list == NULL)
        return;
    destroy_condition_variable(list->condition);
    destroy_monitor(list->monitor);
    size_t dataPtr = 0;
    while(list->size > 0)
        if(remove_from_list_after(listPtr, 0, &dataPtr) == 0)
            destroy_tuple(dataPtr);
    // heap corruption protection, see corresponding note
    // bfree(listPtr);
}

int insert_into_list_after(size_t listPtr, size_t elementPtr, const size_t dataPtr)
{
    list_t *list = dereference_pointer(listPtr);
    if(list == NULL)
        return -1;
    size_t newElementPtr = balloc(sizeof(list_element_t));
    list_element_t *newElement = dereference_pointer(newElementPtr);
    if(newElement == NULL)
        return -1;
    enter_monitor(list->monitor);
    newElement->data = dataPtr;
    list_element_t *element = dereference_pointer(elementPtr);
    list = dereference_pointer(listPtr);
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
    signal_monitor_condition(list->monitor,list->condition);
    leave_monitor(list->monitor);
    return 0;
}

int remove_from_list_after(size_t listPtr, size_t elementPtr, size_t *dataPtr)
{
    list_t *list = dereference_pointer(listPtr);
    if(list == NULL)
        return -1;
    if(list->size == 0)
        return -1;
    list_element_t *element = dereference_pointer(elementPtr);
    size_t oldElement = 0;
    if(element == NULL)
    {
        list_element_t *head = dereference_pointer(list->head);
        *dataPtr = head->data;
        oldElement = list->head;
        list->head = head->next;
    }
    else
    {
        if(is_pointer_null(element->next))
            return -1;
        list_element_t *next = dereference_pointer(element->next);
        *dataPtr = next->data;
        oldElement = element->next;
        element->next = next->next;
    }
    bfree(oldElement);
    --list->size;
    return 0;
}
