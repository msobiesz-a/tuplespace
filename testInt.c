#include <stdio.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#define numberOfTuples 20
int main(int argc, const char *argv[]) {
    printf("Integer Test started \n");
    int handle = init_host();
    ptr_t tuplespace = get_tuplespace();
    int status = 0;
    ptr_t tupleIn;
    ptr_t tupleOut;
    //insert tuples
    for(int i = 0; i <= numberOfTuples; i++)
    {
        tupleIn = create_tuple();
        for(int j = 0; j <=i; j++ )
        {
            add_integer_to_tuple(tupleIn, j);
        }
        if(push_tuple(tuplespace, tupleIn) == -1)
            printf("Błąd \n");
        printf("Add tuple:");
        print_tuple(tupleIn);
    }
    printf("All tuples added\n");
    ptr_t pattern = create_pattern();
    //peek tuples
    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = 0; j <=i; j++ )
        {
            add_integer_to_pattern(pattern, j, EQUAL);
        }
        status = peek_tuple(tuplespace, pattern, &tupleOut);
        printf("Tuple peeked:");
        print_pattern(tupleOut);
    }
    printf("All tuples peeked\n");
    //try to insert same tuples
    for(int i = 0; i <= numberOfTuples; i++)
    {
        tupleIn = create_tuple();
        for(int j = 0; j <=i; j++ )
        {
            add_integer_to_tuple(tupleIn, j);
        }
        if(push_tuple(tuplespace, tupleIn))
            printf("Error, insert same tuple\n");
        printf("Try to add tuple:");
        print_tuple(tupleIn);
    }
    printf("Same tuples not inserted\n");

    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = 1; j <=i+1; j++ )
        {
            add_integer_to_pattern(pattern, j, LESS_THAN);
        }
        status = peek_tuple(tuplespace, pattern, &tupleOut);
        printf("lower tuple peeked  :");
        print_pattern(tupleOut);
    }
    printf("All lower tuples peeked\n");
    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = -1; j <=i-1; j++ )
        {
            add_integer_to_pattern(pattern, j, GREATER);
        }
        status = peek_tuple(tuplespace, pattern, &tupleOut);
        printf("greater tuple peeked  :");
        print_pattern(tupleOut);
    }
    printf("All greater tuples peeked\n");
    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = 0; j <=i; j++ )
        {
            add_integer_to_pattern(pattern, j, EQUAL);
        }
        status = pop_tuple(tuplespace, pattern, &tupleOut);
        printf("Tuple poped:");
        print_pattern(tupleOut);
    }
    printf("All tuples poped\n");
    destroy_patern(pattern);
    free_host(handle);
    printf("Integer Test successed\n");
    return 0;
}
