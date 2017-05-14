#ifndef UXP1A_SHARED_MEMORY_H
#define UXP1A_SHARED_MEMORY_H

#include <stdbool.h>
#include <stdlib.h>


int create_shared_memory(size_t bytes);
void* map_shared_memory(int id);
void unmap_shared_memory(void * ptr);
void destroy_shared_memory(int id);

#endif //UXP1A_SHARED_MEMORY_H
