#include <stdio.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#define numberOfTuples 20
int main(int argc, const char *argv[]) {
    printf("String Test started \n");
    int fixedShmId = create_fixed_shared_memory();
    map_fixed_shared_memory(fixedShmId);
    ptr_t tupleSpacePtr = balloc(sizeof(tuple_space_t));
    initialize_tuple_space(tupleSpacePtr, 10);
    set_start_pointer(tupleSpacePtr);
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
        if(push_tuple(tupleSpacePtr, tupleIn) == -1)
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
            sprintf(string,"%d",j);
            add_string_to_pattern(pattern, string, EQUAL);
        }
        status = peek_tuple(tupleSpacePtr, pattern, &tupleOut);
        printf("Tuple peeked:");
        print_pattern(tupleOut);
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
        if(push_tuple(tupleSpacePtr, tupleIn) != -1)
            printf("Error, insert same tuple\n");
        printf("Try to add tuple:");
        print_tuple(tupleIn);
    }
    printf("Same tuples not inserted\n");

    for(int i = 0; i <= numberOfTuples; i++)
    {
        pattern = create_pattern();
        for(int j = 0; j <=i; j++ )
        {
            sprintf(string,"%d",j);
            add_string_to_tuple(tupleIn, string);
        }
        status = pop_tuple(tupleSpacePtr, pattern, &tupleOut);
        printf("Tuple poped:");
        print_pattern(tupleOut);
    }
    printf("All tuples poped\n");
    destroy_patern(pattern);
    destroy_tuple_space(tupleSpacePtr);
    unmap_fixed_shared_memory();
    destroy_fixed_shared_memory(fixedShmId);
    printf("String Test successed\n");
    return 0;
}
