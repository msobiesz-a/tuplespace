#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
    int handle = init_guest();
    set_id(3);
    ptr_t tuplespace = get_tuplespace();

    int status = 0;
    ptr_t peeked = 0;

    ptr_t pattern = create_pattern();
    add_integer_to_pattern(pattern, 100, EQUAL);
    add_integer_to_pattern(pattern, 100, EQUAL);
    print_pattern(pattern);
    printf("Guest %u requested.\n", get_id());
    status = peek_tuple(tuplespace, pattern, &peeked);
    destroy_patern(pattern);

    if (status == 0)
    {
        printf("Guest %u received.\n", get_id());
        print_tuple(peeked);
    }
    destroy_tuple(peeked);




    free_guest(handle);
    return 0;
}