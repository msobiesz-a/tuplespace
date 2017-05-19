#ifndef UXP1A_SHARED_ALLOCATOR_H
#define UXP1A_SHARED_ALLOCATOR_H

#include <stdbool.h>
#include <stdlib.h>


typedef struct memory_block_t
{
    bool isFree;
    size_t blockSize;
    size_t nextBlock;
    size_t prevBlock;
} memory_block_t;

typedef struct allocation_segment_t
{
    int shmId;
    size_t bytes;
    size_t blockCount;
    size_t usedBlockCount;
    size_t firstBlock;
    size_t lastBlock;
    size_t head;
} allocation_segment_t;

typedef struct fixed_memory_t
{
    int allocShmId;
    int semId;
} fixed_memory_t;


int create_allocation_segment(size_t bytes);
void initialize_allocation_segment(int shmId, size_t bytes);
void initialize_blocks_of_allocation_segment();
void remap_allocation_segment_if_necessary();
void map_allocation_segment(int shmId);
int unmap_allocation_segment();
void destroy_allocation_segment(int memorySegmentId);
void resize_allocation_segment();
void copy_blocks_and_append_new_from_free_space_left(allocation_segment_t *firstSegment,
                                                     allocation_segment_t *secondSegment);
size_t balloc(size_t bytes);
void make_new_block_with_unused_space(size_t blockPtr, size_t bytes, size_t unusedBytes);
void bfree(size_t ptr);
void merge_with_next_block_if_free(size_t blockPtr);
void merge_with_previous_block_if_free(size_t blockPtr);
size_t find_first_fit_free_block(size_t bytes);
memory_block_t *get_block_from_pointer(size_t ptr);
void *dereference_pointer(size_t ptr);
bool is_pointer_null(size_t ptr);
void set_start_pointer(size_t ptr);
size_t get_start_pointer();

int create_fixed_shared_memory();
void map_fixed_shared_memory(int shmId);
void unmap_fixed_shared_memory();
void destroy_fixed_shared_memory(int fixedMemoryId);


#endif //UXP1A_SHARED_ALLOCATOR_H
