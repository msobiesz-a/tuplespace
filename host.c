#include <stdio.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
    int fixedShmId = create_fixed_memory();
    map_fixed_memory(fixedShmId);

    size_t tupleSpacePtr = balloc(sizeof(tuple_space_t));
    initialize_tuple_space(tupleSpacePtr, 10);
    set_head(tupleSpacePtr);

    size_t tuplePtr1 = create_tuple();
    add_integer_to_tuple(tuplePtr1, 10);
    add_integer_to_tuple(tuplePtr1, 20);
    push_tuple(tupleSpacePtr, tuplePtr1);
    print_tuple(tuplePtr1);

    size_t tuplePtr2 = create_tuple();
    add_integer_to_tuple(tuplePtr2, 15);
    add_string_to_tuple(tuplePtr2, "text, more text...");
    push_tuple(tupleSpacePtr, tuplePtr2);
    print_tuple(tuplePtr2);

    write_shm_id_to_file("shmId", fixedShmId);
    printf("Shared memory ID: %d\n", fixedShmId);
    printf("Blocked. Press any key to destroy shared memory...\n");
    getchar();
    destroy_tuple_space(tupleSpacePtr);
    print_memory_info();
    unmap_fixed_memory();
    destroy_fixed_memory(fixedShmId);

    return 0;
}