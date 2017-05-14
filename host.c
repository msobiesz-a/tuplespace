#include <stdio.h>

#include "host.h"
#include "list_t.h"
#include "shared_allocator.h"


int main(int argc, const char *argv[])
{
    create_memory_segment(100);

    size_t listPtr = balloc(sizeof(list_t));
    initialize_list(listPtr);
    set_head(listPtr);
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

    printf("Shared memory ID: %d\n", get_memory_segment_id());
    printf("Press any key to destroy shared memory...\n");
    getchar();

    destroy_list(listPtr);
    destroy_memory_segment(NULL);

    return 0;
}