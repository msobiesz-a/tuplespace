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
    int handle = init_host();
    ptr_t tuplespace = get_tuplespace();
    int status;
    ptr_t peeked = 0;
    ptr_t tupleIn;
    ptr_t tupleOut;
    ptr_t pattern;
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
                if (push_tuple(tuplespace, tupleIn) == -1)
                    printf("same tuple facotry 1\n");
                printf("Factory 1 Add tuple: \n");
                print_tuple(tupleIn);
            }
            printf("Factory 1 Add all tuples: ##########################################\n");
            return 0;
        }
        else {
            sleep(1);
            for(int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                tupleIn = create_tuple();
                for (int j = i; j >= 0; j--) {
                    add_integer_to_tuple(tupleIn, 10+j);
                    sprintf(string,"%d",10+j);
                    add_string_to_tuple(tupleIn, string);
                }
                if (push_tuple(tuplespace, tupleIn) == -1)
                    printf("same tuple factory2\n");
                printf("Factory 2 Add tuple: \n");
                print_tuple(tupleIn);
            }
            printf("Factory 2 Add all tuples: ##########################################\n");
        }
    }
    else {
        //tuple 1 consumer
        if(pid2==0) {

            sleep(1);
            for(int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                pattern = create_pattern();
                for (int j = 0; j <= i; j++) {
                    add_integer_to_pattern(pattern, j, EQUAL);
                    sprintf(string,"%d",j);
                    add_string_to_pattern(pattern, string, EQUAL);
                }
                status = peek_tuple(tuplespace, pattern, &peeked);
                if(status == 0) {
                    printf("consumer 1 peeked tuple: \n");
                    print_tuple(peeked);
                }
                else {
                    printf("consumer 1 error \n");
                }
            }
            printf("Consumer 1 peeked all tuples: ##########################################\n");
            return 0;
        }
            //tuple 2 consumer
        else {

            sleep(1);
            for (int i = 0; i <= numberOfTuples; i++) {
                sleep(1);
                pattern = create_pattern();
                for (int j = 0; j <= i; j++) {
                    add_integer_to_pattern(pattern, j, EQUAL);
                    sprintf(string,"%d",j);
                    add_string_to_pattern(pattern, string, EQUAL);
                }
                status = peek_tuple(tuplespace, pattern, &peeked);
                if (status == 0) {
                    printf("consumer 2 peeked tuple:\n");
                    print_tuple(peeked);
                }
            }
        }
        printf("Consumer 2 peeked all tuples: ##########################################\n");
        return 0;
    }
    waitpid(pid1,0,0);
    waitpid(pid2,0,0);
    destroy_patern(pattern);
    free_host(handle);
    printf("Synchor test succes: \n");
    return 0;
}