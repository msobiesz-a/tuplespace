#include <sys/stat.h>

#include "monitor.h"

#define IPC_PERMISSIONS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

const int SEMAPHORE_INITIAL_VALUE = 1;


int create_semaphore(int value)
{
    int semId = semget(IPC_PRIVATE, 1, IPC_PERMISSIONS | IPC_CREAT);
    if(semId == -1)
    {
        printf("error: semget\n");
        exit(1);
    }
    semun_t init = {.val = value};
    if(semctl(semId, 0, SETVAL, init) == -1)
    {
        printf("error: semctl (initialize)\n");
        exit(1);
    }
    return semId;
}

int destroy_semaphore(int semId)
{
    if(semctl(semId, 0, IPC_RMID == -1))
    {
        printf("error: semctl (destroy)\n");
        exit(1);
    }
    return 0;
}

void acquire_semaphore(int semId)
{
    sembuf_t semOp = {.sem_num = 0,
            .sem_op = -1,
            .sem_flg = 0};
    if(semop(semId, &semOp, 1) == -1)
    {
        printf("error: semop (wait)\n");
        exit(1);
    }
}

void release_semaphore(int semId)
{
    sembuf_t semOp = {.sem_num = 0,
            .sem_op = 1,
            .sem_flg = 0};
    if(semop(semId, &semOp, 1) == -1)
    {
        printf("error: semop (post)\n");
        exit(1);
    }
}

ptr_t create_condition_variable()
{
    ptr_t condVarPtr = balloc(sizeof(condition_variable_t));
    condition_variable_t *condVar = deref_ptr(condVarPtr);
    condVar->semId = create_semaphore(0);
    condVar->waitingCount = 0;
    return condVarPtr;
}

void destroy_condition_variable(ptr_t condVarPtr)
{
    condition_variable_t *condVar = deref_ptr(condVarPtr);
    destroy_semaphore(condVar->semId);
    bfree(condVarPtr);
}

void wait_on_condition_variable(ptr_t condVarPtr)
{
    condition_variable_t *condVar = deref_ptr(condVarPtr);
    acquire_semaphore(condVar->semId);
}

bool signal_condition_variable(ptr_t condVarPtr)
{
    condition_variable_t *condVar = deref_ptr(condVarPtr);
    if(condVar->waitingCount)
    {
        --condVar->waitingCount;
        release_semaphore(condVar->semId);
        return true;
    }
    else
        return false;
}

ptr_t create_monitor()
{
    ptr_t monitorPtr = balloc(sizeof(monitor_t));
    monitor_t *monitor = deref_ptr(monitorPtr);
    monitor->semId = create_semaphore(1);
    return monitorPtr;
}

void destroy_monitor(ptr_t monitorPtr)
{
    monitor_t *monitor = deref_ptr(monitorPtr);
    destroy_semaphore(monitor->semId);
    bfree(monitorPtr);
}

void enter_monitor(ptr_t monitorPtr)
{
    monitor_t *monitor = deref_ptr(monitorPtr);
    acquire_semaphore(monitor->semId);
}

void leave_monitor(ptr_t monitorPtr)
{
    monitor_t *monitor = deref_ptr(monitorPtr);
    release_semaphore(monitor->semId);
}

void wait_on_monitor_condition(ptr_t monitorPtr, ptr_t condVarPtr)
{
    condition_variable_t *condVar = deref_ptr(condVarPtr);
    ++condVar->waitingCount;
    leave_monitor(monitorPtr);
    wait_on_condition_variable(condVarPtr);
}

void signal_monitor_condition(ptr_t monitorPtr, ptr_t condVarPtr)
{
    if(signal_condition_variable(condVarPtr))
        enter_monitor(monitorPtr);
}