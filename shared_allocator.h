#ifndef UXP1A_SHARED_ALLOCATOR_H
#define UXP1A_SHARED_ALLOCATOR_H

#include <stdbool.h>
#include <stdlib.h>


typedef struct block_descriptor_t
{
    bool isFree;
    size_t blockSize;
    size_t nextBlock;
    size_t prevBlock;
} block_descriptor_t;

typedef struct segment_descriptor_t
{
    int shmId;
    size_t bytes;
    size_t blockCount;
    size_t usedBlockCount;
    size_t firstBlock;
    size_t lastBlock;
    size_t head;
} segment_descriptor_t;

void create_memory_segment(size_t initialCapacityInBytes);
int get_memory_segment_id();
void map_memory_segment(int shmId);
void unmap_memory_segment();
void destroy_memory_segment(segment_descriptor_t **shmPtr);
void resize_memory_segment();
size_t balloc(size_t bytes);
size_t find_first_fit_free_block(size_t bytes);
block_descriptor_t *get_block_from_pointer(size_t ptr);
void *dereference_pointer(size_t ptr);
bool is_pointer_null(size_t ptr);
void set_head(size_t ptr);
size_t get_head();
void bfree(size_t ptr);

#endif //UXP1A_SHARED_ALLOCATOR_H
