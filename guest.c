#include <stdio.h>

#include "guest.h"
#include "list_t.h"
#include "shared_allocator.h"


int main(int argc, const char *argv[])
{
    int shmId = -1;
    printf("Enter global shared memory handle: ");
    if(!scanf("%d", &shmId))
    {
        printf("error: scanf\n");
        exit(1);
    }
    map_memory_segment(shmId);

    size_t listPtr = get_head();
    list_t *list = dereference_pointer(listPtr);
    for(int i = 0; i < 10; ++i)
    {
        size_t retrievedIntegerPtr = 0;
        remove_from_list_after(listPtr, 0, &retrievedIntegerPtr);
        int *retrievedInteger = dereference_pointer(retrievedIntegerPtr);
        printf("Retrieved: %d\n", *retrievedInteger);
    }
    printf("List size: %u\n", (unsigned int) list->size);

    unmap_memory_segment();

    return 0;
}