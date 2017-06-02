#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"


unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void write_handle_to_file(const char *filename, int shmId)
{
    FILE *shmIdFile = fopen(filename, "w+");
    if(shmIdFile)
    {
        fprintf(shmIdFile, "%d\n", shmId);
        fclose(shmIdFile);
    }
}

int read_handle_from_file(const char *filename)
{
    int shmId = -1;
    FILE *shmIdFile = fopen(filename, "r");
    if(shmIdFile)
    {
        fscanf(shmIdFile, "%d", &shmId);
        fclose(shmIdFile);
    }
    return shmId;
}

ptr_t malloc_l(ptr_t bytes)
{
    return (ptr_t) malloc(bytes);
}

void free_l(ptr_t ptr)
{
    free((void *) ptr);
}

void *deref_l(ptr_t ptr)
{
    return (void *) ptr;
}

bool is_null_l(ptr_t ptr)
{
    return ((void *) ptr == NULL);
}