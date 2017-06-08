#include <stdio.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#define numberOfTuples 20
int main(int argc, const char *argv[]) {
    printf("String Test started \n");
    int handle = init_host();
    ptr_t tuplespace = get_tuplespace();
    int status = 0;
    ptr_t tupleIn;
    ptr_t tupleOut;
    char string [3] ;
    //insert tuples
    for(int i = 0; i <= numberOfTuples; i++)
    {
        tupleIn = create_tuple();
        for(int j = 0; j <=i; j++ )
        {
            sprintf(string,"%d",j);
            add_string_to_tuple(tupleIn, string);
        }
        if(push_tuple(tuplespace, tupleIn) == -1)
            printf("Błąd \n");
        printf("Add tuple:");
        print_tuple(tupleIn);
        destroy_tuple(tupleIn);
    }
    printf("All tuples added\n");
    ptr_t pattern = create_pattern();
    //peek tuples
    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = 0; j <=i; j++ )
        {
            sprintf(string,"%d",j);
            add_string_to_pattern(pattern, string, EQUAL);
        }
        status = peek_tuple(tuplespace, pattern, &tupleOut);
        printf("Tuple peeked:");
        print_pattern(tupleOut);
        destroy_tuple(tupleOut);
        destroy_patern(pattern);
    }
    printf("All tuples peeked\n");
    //try to insert same tuples
    for(int i = 0; i <= numberOfTuples; i++)
    {
        tupleIn = create_tuple();
        for(int j = 0; j <=i; j++ ){
            sprintf(string,"%d",j);
            add_string_to_tuple(tupleIn, string);
        }
        if(push_tuple(tuplespace, tupleIn) != 1)
            printf("Error, insert same tuple\n");
        printf("Try to add tuple:");
        print_tuple(tupleIn);
        destroy_tuple(tupleIn);
    }
    printf("Same tuples not inserted\n");

    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = 0; j <=i; j++ )
        {
            sprintf(string,"%d",j);
            add_string_to_pattern(pattern, string,EQUAL);
        }
        status = pop_tuple(tuplespace, pattern, &tupleOut);
        printf("Tuple poped:");
        print_pattern(tupleOut);
        destroy_tuple(tupleOut);
        destroy_patern(pattern);
    }
    printf("All tuples poped\n");
    destroy_patern(pattern);
    free_host(handle);
    printf("String Test successed\n");
    return 0;
}
