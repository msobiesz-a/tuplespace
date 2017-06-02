#ifndef UXP1A_SHARED_ALLOCATOR_H
#define UXP1A_SHARED_ALLOCATOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

typedef struct memory_block_t
{
    bool isFree;
    size_t blockSize;
    ptr_t nextBlock;
    ptr_t prevBlock;
} memory_block_t;

typedef struct allocation_segment_t
{
    int shmId;
    size_t bytes;
    size_t blockCount;
    size_t usedBlockCount;
    ptr_t firstBlock;
    ptr_t lastBlock;
    ptr_t head;
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
ptr_t balloc(size_t bytes);
void make_new_block_with_unused_space(ptr_t blockPtr, size_t bytes, size_t unusedBytes);
void bfree(ptr_t ptr);
void merge_with_next_block_if_free(ptr_t blockPtr);
void merge_with_previous_block_if_free(ptr_t blockPtr);
ptr_t find_first_fit_free_block(size_t bytes);
memory_block_t *get_block_from_pointer(ptr_t ptr);
void *deref_ptr(ptr_t ptr);
bool is_ptr_null(ptr_t ptr);
void set_start_pointer(ptr_t ptr);
ptr_t get_start_pointer();

int create_fixed_shared_memory();
void map_fixed_shared_memory(int shmId);
void unmap_fixed_shared_memory();
void destroy_fixed_shared_memory(int fixedMemoryId);

int init_host();
void free_host(int handle);
int init_guest();
void free_guest(int handle);

ptr_t get_tuplespace();

#endif //UXP1A_SHARED_ALLOCATOR_H
