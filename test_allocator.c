#include <stdio.h>

#include "list_t.h"
#include "shared_allocator.h"
#include "test_allocator.h"


int main(int argc, const char *argv[])
{
    create_memory_segment(10);

    size_t listPtr = balloc(sizeof(list_t));
    initialize_list(listPtr);

    list_t *list = dereference_pointer(listPtr);

    for(int i = 0; i < 10; ++i) {
        size_t integerPtr = balloc(sizeof(int));
        int *integer = dereference_pointer(integerPtr);
        *integer = i;
        insert_into_list_after(listPtr, 0, integerPtr);
        integer = dereference_pointer(integerPtr);
        printf("Put: %d\n", *integer);
    }
    printf("List size: %u\n", (unsigned int) list->size);

    for(int i = 0; i < 10; ++i)
    {
        size_t retrievedIntegerPtr = 0;
        remove_from_list_after(listPtr, 0, &retrievedIntegerPtr);
        int *retrievedInteger = dereference_pointer(retrievedIntegerPtr);
        printf("Retrieved: %d\n", *retrievedInteger);
    }
    printf("List size: %u\n", (unsigned int) list->size);

    destroy_list(listPtr);
    destroy_memory_segment(NULL);

    return 0;
}