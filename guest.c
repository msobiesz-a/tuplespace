#include <stdio.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
    int shmId = read_shm_id_from_file("shmId");
    printf("Shared memory ID: %d\n", shmId);

    map_fixed_memory(shmId);

    size_t tupleSpacePtr = get_head();
    int status = 0;
    size_t peeked1 = 0;
    size_t peeked2 = 0;
    size_t peeked3 = 0;

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

    size_t patternPtr3 = create_pattern();
    add_string_to_pattern(patternPtr3, "very long character sequence, very long character sequence, very long character"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,", EQUAL);
    add_integer_to_pattern(patternPtr3, 1, EQUAL);
    add_integer_to_pattern(patternPtr3, 2, EQUAL);
    add_integer_to_pattern(patternPtr3, 3, EQUAL);
    add_integer_to_pattern(patternPtr3, 4, EQUAL);
    add_integer_to_pattern(patternPtr3, 5, EQUAL);
    add_integer_to_pattern(patternPtr3, 6, EQUAL);
    add_integer_to_pattern(patternPtr3, 7, EQUAL);
    add_integer_to_pattern(patternPtr3, 8, EQUAL);
    add_integer_to_pattern(patternPtr3, 9, EQUAL);
    add_integer_to_pattern(patternPtr3, 10, EQUAL);
    add_integer_to_pattern(patternPtr3, 11, EQUAL);
    add_string_to_pattern(patternPtr3, "very long character sequence, very long character sequence, very long character"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,"
            "very long character sequence, very long character sequence, very long character sequence,", EQUAL);
    add_integer_to_pattern(patternPtr3, 12, EQUAL);
    add_integer_to_pattern(patternPtr3, 13, EQUAL);
    add_integer_to_pattern(patternPtr3, 14, EQUAL);
    add_integer_to_pattern(patternPtr3, 15, EQUAL);
    add_integer_to_pattern(patternPtr3, 16, EQUAL);
    add_integer_to_pattern(patternPtr3, 17, EQUAL);
    add_integer_to_pattern(patternPtr3, 18, EQUAL);
    add_integer_to_pattern(patternPtr3, 19, EQUAL);
    add_integer_to_pattern(patternPtr3, 20, EQUAL);
    add_integer_to_pattern(patternPtr3, 21, EQUAL);
    status = peek_tuple(tupleSpacePtr, patternPtr3, &peeked3);
    if(status == 0)
        print_tuple(peeked3);

    destroy_patern(patternPtr1);
    destroy_patern(patternPtr2);
    destroy_patern(patternPtr3);
    unmap_fixed_memory();

    return 0;
}