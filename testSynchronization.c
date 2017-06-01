#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"
#include "utils.h"

#define numberOfTuples 5
int main(int argc, const char *argv[]) {
    printf("Synchronization Test started \n");
    int fixedShmId = create_fixed_shared_memory();
    map_fixed_shared_memory(fixedShmId);
    size_t tupleSpacePtr = balloc(sizeof(tuple_space_t));
    initialize_tuple_space(tupleSpacePtr, 10);
    set_start_pointer(tupleSpacePtr);
    int status;
    size_t peeked = 0;
    size_t tupleIn;
    size_t tupleOut;
    size_t pattern;
    char string [3] ;
    pid_t pid1 = fork();
    pid_t pid2 = fork();
    if (pid1 == 0) {
        //tuple 1 factory
        if(pid2==0) {
            sleep(1);
            for(int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                tupleIn = create_tuple();
                for (int j = 0; j <= i; j++) {
                    add_integer_to_tuple(tupleIn, j);
                    sprintf(string,"%d",j);
                    add_string_to_tuple(tupleIn, string);
                }
                if (push_tuple(tupleSpacePtr, tupleIn) == -1)
                    printf("same tuple facotry 1\n");
                printf("Factory 1 Add tuple: \n");
                print_tuple(tupleIn);
            }
            return 0;
        }
        else {
            sleep(1);
            for(int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                tupleIn = create_tuple();
                for (int j = i; j >= 0; j--) {
                    add_integer_to_tuple(tupleIn, j);
                    sprintf(string,"%d",j);
                    add_string_to_tuple(tupleIn, string);
                }
                if (push_tuple(tupleSpacePtr, tupleIn) == -1)
                    printf("same tuple factory2\n");
                printf("Factory 2 Add tuple: \n");
                print_tuple(tupleIn);
            }
        }
    }
    else {
        //tuple 1 consumer
        if(pid2==0) {
            for(int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                pattern = create_pattern();
                for (int j = 0; j <= i; j++) {
                    add_integer_to_pattern(pattern, j, EQUAL);
                    sprintf(string,"%d",j);
                    add_string_to_pattern(pattern, string, EQUAL);
                }
                status = peek_tuple(tupleSpacePtr, pattern, &peeked);
                if(status == 0) {
                    printf("consumer 1 peeked tuple: \n");
                    print_tuple(peeked);
                }
                else {
                    printf("consumer 1 error \n");
                }
            }
            return 0;
        }
            //tuple 2 consumer
        else {
            sleep(3);
            for (int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                pattern = create_pattern();
                for (int j = i; j >= 0; j--) {
                    add_integer_to_pattern(pattern, j, EQUAL);
                    sprintf(string, "%d", j);
                    add_string_to_pattern(pattern, string, EQUAL);
                }
                status = peek_tuple(tupleSpacePtr, pattern, &peeked);
                if (status == 0) {
                    printf("consumer 2 peeked tuple:\n");
                    print_tuple(peeked);
                }
            }

        }
    }
    waitpid(pid1,0,0);
    waitpid(pid2,0,0);
    destroy_patern(pattern);
    destroy_tuple_space(tupleSpacePtr);
    unmap_fixed_shared_memory();
    destroy_fixed_shared_memory(fixedShmId);
    return 0;
}