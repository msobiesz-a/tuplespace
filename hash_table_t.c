#include <string.h>

#include "hash_table_t.h"
#include "shared_allocator.h"
#include "tuple_t.h"

int initialize_hash_table(ptr_t hashTablePtr, ptr_t initialBucketsCount)
{
    hash_table_t *hashTable = deref_ptr(hashTablePtr);
    hashTable->size = 0;
    hashTable->bucketsCount = initialBucketsCount;
    ptr_t bucketsPtr = balloc(initialBucketsCount * sizeof(list_t));
    hashTable = deref_ptr(hashTablePtr);
    hashTable->bucketsPtr = bucketsPtr;
    if(is_ptr_null(hashTable->bucketsPtr))
        return -1;
    for(ptr_t i = 0; i < hashTable->bucketsCount; ++i)
        initialize_list(hashTable->bucketsPtr + (i * sizeof(list_t)));
    return 0;
}

void destroy_hash_table(ptr_t hashTablePtr)
{
    hash_table_t *hashTable = deref_ptr(hashTablePtr);
    for(ptr_t i = 0; i < hashTable->bucketsCount; ++i)
        destroy_list(hashTable->bucketsPtr + (i * sizeof(list_t)));
    bfree(hashTable->bucketsPtr);
    bfree(hashTablePtr);
}

int put_into_hash_table(ptr_t hashTablePtr, const ptr_t tuplePtr) {
    int status = 0;
    if (is_in_hash_table(hashTablePtr, tuplePtr)) {
        return 1;
    }
    hash_table_t *hashTable = deref_ptr(hashTablePtr);
    ptr_t bucket = (ptr_t) hash_tuple(tuplePtr) % hashTable->bucketsCount;

    if((status = insert_into_list_after(hashTable->bucketsPtr + (bucket * sizeof(list_t)), 0, tuplePtr)) == 0) {
        hashTable = deref_ptr(hashTablePtr);
        ++hashTable->size;
        list_t *list = deref_ptr(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
        leave_monitor(list->monitor);
        broadcast_monitor_condition(list->monitor,list->condition);
    }
    return status;
}

int get_from_hash_table(const ptr_t hashTablePtr, const ptr_t patternPtr, ptr_t *tuplePtr)
{
    hash_table_t *hashTable = deref_ptr(hashTablePtr);
    ptr_t bucket = (ptr_t) hash_tuple(patternPtr) % hashTable->bucketsCount;
    list_t *list = deref_ptr(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
    enter_monitor(list->monitor);
    while(true) {
        for (ptr_t elementPtr = list->head; !is_ptr_null(elementPtr);) {
            list_element_t *element = deref_ptr(elementPtr);
            if (does_tuple_match_pattern(deref_ptr(element->data), deref_l(patternPtr))) {
                *tuplePtr = clone_into_local_memory(element->data);
                signal_monitor_condition(list->monitor,list->condition);
                leave_monitor(list->monitor);
                return 0;
            }
            elementPtr = element->next;
        }
        list = deref_ptr(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
        wait_on_monitor_condition(list->monitor,list->condition);
    }
}

bool is_in_hash_table(const ptr_t hashTablePtr, const ptr_t tuplePtr)
{
    hash_table_t *hashTable = deref_ptr(hashTablePtr);
    ptr_t bucket = hash_tuple(tuplePtr) % hashTable->bucketsCount;
    list_t *list = deref_ptr(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
    ptr_t elementPtr = list->head;
    while(!is_ptr_null(elementPtr))
    {
        list_element_t *element = deref_ptr(elementPtr);
        if(do_tuples_match(deref_ptr(element->data), deref_l(tuplePtr)))
            return true;
        elementPtr = element->next;
    }
    return false;
}

int remove_from_hash_table(ptr_t hashTablePtr, const ptr_t patternPtr, ptr_t *tuplePtr)
{
    hash_table_t *hashTable = deref_ptr(hashTablePtr);
    ptr_t bucket = (ptr_t) hash_tuple(patternPtr) % hashTable->bucketsCount;
    list_t *list = deref_ptr(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
    ptr_t previousPtr = 0;
    for(ptr_t elementPtr = list->head; !is_ptr_null(elementPtr);)
    {
        list_element_t *element = deref_ptr(elementPtr);
        if(does_tuple_match_pattern(deref_ptr(element->data), deref_l(patternPtr)))
        {
            if(remove_from_list_after(hashTable->bucketsPtr + (bucket * sizeof(list_t)), previousPtr, tuplePtr) == 0)
            {
                --hashTable->size;
                return 0;
            }
            else
                return -1;

        }
        previousPtr = elementPtr;
        elementPtr = element->next;

    }
    return -1;
}