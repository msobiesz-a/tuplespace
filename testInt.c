#include <stdio.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
int main(int argc, const char *argv[]) {
    printf("Integer Test started \n");
    int fixedShmId = create_fixed_shared_memory();
    map_fixed_shared_memory(fixedShmId);
    size_t tupleSpacePtr = balloc(sizeof(tuple_space_t));
    initialize_tuple_space(tupleSpacePtr, 10);
    set_start_pointer(tupleSpacePtr);
    int status = 0;
    size_t tupleIn;
    size_t tupleOut;
    for(int i = 0; i <= 100; i++)
    {
        tupleIn = create_tuple();
        for(int j = 0; j <=i; j++ )
        {
            add_integer_to_tuple(tupleIn, j);
        }
        push_tuple(tupleSpacePtr, tupleIn);
        printf("Add tuple:");
        print_tuple(tupleIn);
    }
    printf("All tuples added");
    size_t pattern = create_pattern();
    for(int i = 0; i <= 100; i++)
    {
        pattern = create_pattern();
        for(int j = 0; j <=i; j++ )
        {
            add_integer_to_pattern(pattern, j, EQUAL);
        }
        status = peek_tuple(tupleSpacePtr, pattern, &tupleOut);
        printf("Tuple peeked:");
        print_pattern(tupleOut);
    }
    destroy_patern(pattern);
    destroy_tuple_space(tupleSpacePtr);
    unmap_fixed_shared_memory();
    destroy_fixed_shared_memory(fixedShmId);
    printf("Integer Test successed");
    return 0;
}
