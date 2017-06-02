#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#include "utils.h"


int main(int argc, const char *argv[])
{
    int handle = init_host();
    ptr_t tuplespace = get_tuplespace();

    write_handle_to_file("shmId", handle);
    printf("Handle: %d\n", handle);

    size_t i = 50;
    srand (time(NULL));
    while(i--)
    {
        sleep(rand() % 6);
        ptr_t tuple = create_tuple();
        add_integer_to_tuple(tuple, 100);
        add_integer_to_tuple(tuple, 100);
        print_tuple(tuple);
        push_tuple(tuplespace, tuple);
        destroy_tuple(tuple);
    }


    printf("Blocked. Press any key to exit...\n");
    getchar();
    free_host(handle);
    return 0;
}