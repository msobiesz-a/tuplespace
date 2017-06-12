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
    set_id(0);
    ptr_t tuplespace = get_tuplespace();

    write_handle_to_file("shmId", handle);
    printf("Handle: %d\n", handle);



    for(size_t i = 0; i < 3; ++i)
    {
        getchar();
        ptr_t tuple = create_tuple();
        add_integer_to_tuple(tuple, 100);
        add_integer_to_tuple(tuple, 100);
        print_tuple(tuple);
        int status = push_tuple(tuplespace, tuple);
        printf("Host pushed tuple with status: %d\n", status);
        destroy_tuple(tuple);
    }

    printf("Blocked. Press any key to exit...\n");
    getchar();
    free_host(handle);
    return 0;
}