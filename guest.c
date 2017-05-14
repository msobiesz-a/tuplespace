#include <stdio.h>

#include "guest.h"
#include "list_t.h"
#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"


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

    size_t tupleSpacePtr = get_head();
    int status = 0;
    size_t peeked1 = 0;
    size_t peeked2 = 0;

    size_t patternPtr1 = create_pattern();
    add_integer_to_pattern(patternPtr1, 10, EQUAL);
    add_integer_to_pattern(patternPtr1, 20, EQUAL);
    status = peek_tuple(tupleSpacePtr, patternPtr1, &peeked1);
    if(status == 0)
        print_tuple(peeked1);

    size_t patternPtr2 = create_pattern();
    add_integer_to_pattern(patternPtr2, 0, ANY);
    add_string_to_pattern(patternPtr2, "text, more text...", EQUAL);
    status = peek_tuple(tupleSpacePtr, patternPtr2, &peeked2);
    if(status == 0)
        print_tuple(peeked2);

    unmap_memory_segment();

    return 0;
}