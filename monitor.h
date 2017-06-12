#ifndef UXP1A_MONITOR_H
#define UXP1A_MONITOR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#include "shared_allocator.h"


const int SEMAPHORE_INITIAL_VALUE;

typedef struct sembuf sembuf_t;

typedef union semun {
    int  val;
    struct  semid_ds *buf;
    ushort  *array;
} semun_t;

typedef struct condition_variable_t
{
    int condSemId;
    int exclSemId;
    int handSemId;
    unsigned int waitingCount;
    unsigned int waitIndex;
    unsigned int wakeIndex;
} condition_variable_t;

typedef struct monitor_t
{
    int semId;
} monitor_t;

int create_semaphore(int value);
int destroy_semaphore(int semId);
void acquire_semaphore(int semId);
void release_semaphore(int semId);

ptr_t create_condition_variable();
void destroy_condition_variable(ptr_t condVarPtr);

ptr_t create_monitor();
void destroy_monitor(ptr_t monitorPtr);
void enter_monitor(ptr_t monitorPtr);
void leave_monitor(ptr_t monitorPtr);
void wait_on_monitor_condition(ptr_t monitorPtr, ptr_t condVarPtr);
void signal_monitor_condition(ptr_t monitorPtr, ptr_t condVarPtr);
void broadcast_monitor_condition(ptr_t monitorPtr, ptr_t condVarPtr);

#endif //UXP1A_MONITOR_H
