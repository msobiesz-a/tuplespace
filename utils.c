#include <stdio.h>

#include "utils.h"


unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void write_shm_id_to_file(const char *filename, int shmId)
{
    FILE *shmIdFile = fopen(filename, "w+");
    if(shmIdFile)
    {
        fprintf(shmIdFile, "%d\n", shmId);
        fclose(shmIdFile);
    }
}

int read_shm_id_from_file(const char *filename)
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