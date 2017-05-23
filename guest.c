#include <stdio.h>
#include <zconf.h>
#include <sys/wait.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
    pid_t pid  = fork();
    int shmId = read_shm_id_from_file("shmId");
    printf("Shared memory ID: %d\n", shmId);
    map_fixed_shared_memory(shmId);
    size_t tupleSpacePtr = get_start_pointer();
    int status = 0;
    size_t peeked1 = 0;
    size_t peeked2 = 0;

    if(pid == 0)
    {
        sleep(3);
        size_t tuplePtr1 = create_tuple();
        add_integer_to_tuple(tuplePtr1, 10);
        add_integer_to_tuple(tuplePtr1, 20);
        add_integer_to_tuple(tuplePtr1, 30);
        push_tuple(tupleSpacePtr, tuplePtr1);
        unmap_fixed_shared_memory();
        return 0;
    }
    size_t patternPtr1 = create_pattern();
    add_integer_to_pattern(patternPtr1, 10, EQUAL);
    add_integer_to_pattern(patternPtr1, 20, EQUAL);
    status = peek_tuple(tupleSpacePtr, patternPtr1, &peeked1);
    if(status == 0)
        print_tuple(peeked1);

    size_t patternPtr2 = create_pattern();
    add_integer_to_pattern(patternPtr2, 0, ANY);
    add_integer_to_pattern(patternPtr2, 0, ANY);
    add_integer_to_pattern(patternPtr2, 0, ANY);
    //add_string_to_pattern(patternPtr2, "text, more text...", EQUAL);
    status = peek_tuple(tupleSpacePtr, patternPtr2, &peeked2);
    if(status == 0)
        print_tuple(peeked2);


    destroy_patern(patternPtr1);
    destroy_patern(patternPtr2);
    unmap_fixed_shared_memory();

    return 0;
}