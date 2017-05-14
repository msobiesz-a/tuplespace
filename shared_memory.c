#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "shared_memory.h"

#define IPC_PERMISSIONS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)


int create_shared_memory(size_t bytes)
{
    int shmId = shmget(IPC_PRIVATE, bytes, IPC_CREAT | IPC_PERMISSIONS);
    if(shmId == -1) {
        printf("error: shmget\n");
        exit(1);
    }
    return shmId;
}

void* map_shared_memory(int id)
{
    void *ptr = shmat(id, NULL, 0);
    if(ptr == (void *) -1) {
        printf("error: shmat\n");
        exit(1);
    }
    return ptr;

}

void unmap_shared_memory(void * ptr)
{
    if(shmdt(ptr) == -1) {
        printf("error: shmdt\n");
        exit(1);
    }
}

void destroy_shared_memory(int id)
{
    if(shmctl(id, IPC_RMID, 0) == -1) {
        printf("error: shmctl\n");
        exit(1);
    }
}