#include <string.h>

#include "hash_table_t.h"
#include "shared_allocator.h"
#include "tuple_t.h"

int initialize_hash_table(size_t hashTablePtr, size_t initialBucketsCount)
{
    hash_table_t *hashTable = dereference_pointer(hashTablePtr);
    hashTable->size = 0;
    hashTable->bucketsCount = initialBucketsCount;
    size_t bucketsPtr = balloc(initialBucketsCount * sizeof(list_t));
    hashTable = dereference_pointer(hashTablePtr);
    hashTable->bucketsPtr = bucketsPtr;
    if(is_pointer_null(hashTable->bucketsPtr))
        return -1;
    for(size_t i = 0; i < hashTable->bucketsCount; ++i)
        initialize_list(hashTable->bucketsPtr + (i * sizeof(list_t)));
    return 0;
}

void destroy_hash_table(size_t hashTablePtr)
{
    hash_table_t *hashTable = dereference_pointer(hashTablePtr);
    for(size_t i = 0; i < hashTable->bucketsCount; ++i)
        destroy_list(hashTable->bucketsPtr + (i * sizeof(list_t)));
    bfree(hashTable->bucketsPtr);
    bfree(hashTablePtr);
}

int put_into_hash_table(size_t hashTablePtr, const size_t tuplePtr) {
    int status = 0;
    if (is_in_hash_table(hashTablePtr, tuplePtr)) {
        return 1;
    }
    hash_table_t *hashTable = dereference_pointer(hashTablePtr);
    size_t bucket = (size_t) hash_tuple(tuplePtr) % hashTable->bucketsCount;

    if((status = insert_into_list_after(hashTable->bucketsPtr + (bucket * sizeof(list_t)), 0, tuplePtr)) == 0)
        ++hashTable->size;
    return status;
}

int get_from_hash_table(const size_t hashTablePtr, const size_t patternPtr, size_t *tuplePtr)
{
    hash_table_t *hashTable = dereference_pointer(hashTablePtr);
    size_t bucket = (size_t) hash_tuple(patternPtr) % hashTable->bucketsCount;
    list_t *list = dereference_pointer(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
    enter_monitor(list->monitor);
    while(true) {
        for (size_t elementPtr = list->head; !is_pointer_null(elementPtr);) {
            list_element_t *element = dereference_pointer(elementPtr);
            if (does_tuple_match_pattern(dereference_pointer(element->data), dereference_pointer(patternPtr))) {
                *tuplePtr = element->data;
                leave_monitor(list->monitor);
                return 0;
            }
            elementPtr = element->next;
        }
        wait_on_monitor_condition(list->monitor,list->condition);
    }
}

bool is_in_hash_table(const size_t hashTablePtr, const size_t tuplePtr)
{
    hash_table_t *hashTable = dereference_pointer(hashTablePtr);
    size_t bucket = (size_t) hash_tuple(tuplePtr) % hashTable->bucketsCount;
    list_t *list = dereference_pointer(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
    size_t elementPtr = list->head;
    while(!is_pointer_null(elementPtr))
    {
        list_element_t *element = dereference_pointer(elementPtr);
        if(do_tuples_match(dereference_pointer(element->data), dereference_pointer(tuplePtr)))
            return true;
        elementPtr = element->next;
    }
    return false;
}

int remove_from_hash_table(size_t hashTablePtr, const size_t patternPtr, size_t *tuplePtr)
{
    hash_table_t *hashTable = dereference_pointer(hashTablePtr);
    size_t bucket = (size_t) hash_tuple(patternPtr) % hashTable->bucketsCount;
    list_t *list = dereference_pointer(hashTable->bucketsPtr + (bucket * sizeof(list_t)));
    size_t previousPtr = 0;
    for(size_t elementPtr = list->head; !is_pointer_null(elementPtr);)
    {
        list_element_t *element = dereference_pointer(elementPtr);
        if(does_tuple_match_pattern(dereference_pointer(element->data), dereference_pointer(patternPtr)))
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
