#include <printf.h>
#include <stdio.h>
#include <string.h>

#include "monitor.h"
#include "shared_allocator.h"
#include "shared_memory.h"


static const size_t INITIAL_ALLOCATION_SEGMENT_SIZE = 8192;
static const size_t RESIZE_FACTOR = 2;
static const size_t ALLOCATION_SEGMENT_HEADER_SIZE = sizeof(allocation_segment_t);
static const size_t BLOCK_HEADER_SIZE = sizeof(memory_block_t);
static const size_t MINIMAL_MEMORY_BLOCK_SIZE = 1;

static fixed_memory_t *fixedSharedMemory;
static allocation_segment_t *mappedAllocationSegment;

int create_fixed_shared_memory()
{
    int shmId = create_shared_memory(sizeof(fixed_memory_t));
    fixed_memory_t *shmPtr = map_shared_memory(shmId);
    shmPtr->allocShmId = create_allocation_segment(INITIAL_ALLOCATION_SEGMENT_SIZE);
    shmPtr->semId = create_semaphore(1);
    unmap_shared_memory(shmPtr);
    return shmId;
}

void map_fixed_shared_memory(int shmId)
{
    fixedSharedMemory = map_shared_memory(shmId);
    map_allocation_segment(fixedSharedMemory->allocShmId);
}

void unmap_fixed_shared_memory()
{
    unmap_allocation_segment();
    unmap_shared_memory(fixedSharedMemory);
    fixedSharedMemory = NULL;
}

void destroy_fixed_shared_memory(int fixedMemoryId)
{
    map_fixed_shared_memory(fixedMemoryId);
    int semId = fixedSharedMemory->semId;
    int memShmId = fixedSharedMemory->allocShmId;
    unmap_fixed_shared_memory();
    destroy_semaphore(semId);
    destroy_allocation_segment(memShmId);
    destroy_shared_memory(fixedMemoryId);
}

int create_allocation_segment(size_t bytes)
{
    if(bytes < BLOCK_HEADER_SIZE + MINIMAL_MEMORY_BLOCK_SIZE)
        bytes = BLOCK_HEADER_SIZE + MINIMAL_MEMORY_BLOCK_SIZE;
    int shmId = create_shared_memory(bytes + ALLOCATION_SEGMENT_HEADER_SIZE);
    map_allocation_segment(shmId);
    initialize_allocation_segment(shmId, bytes);
    return unmap_allocation_segment();
}

void initialize_allocation_segment(int shmId, size_t bytes)
{
    mappedAllocationSegment->shmId = shmId;
    mappedAllocationSegment->bytes = bytes;
    mappedAllocationSegment->blockCount = 1;
    mappedAllocationSegment->usedBlockCount = 0;
    mappedAllocationSegment->firstBlock = ALLOCATION_SEGMENT_HEADER_SIZE;
    mappedAllocationSegment->lastBlock = ALLOCATION_SEGMENT_HEADER_SIZE;
    mappedAllocationSegment->head = 0;
    initialize_blocks_of_allocation_segment();
}

void initialize_blocks_of_allocation_segment()
{
    memory_block_t *firstBlock = get_block_from_pointer(mappedAllocationSegment->firstBlock);
    firstBlock->blockSize = mappedAllocationSegment->bytes - BLOCK_HEADER_SIZE;
    firstBlock->isFree = true;
    firstBlock->nextBlock = 0;
    firstBlock->prevBlock = 0;
}

void map_allocation_segment(int shmId)
{
    mappedAllocationSegment = map_shared_memory(shmId);
}

void remap_allocation_segment_if_necessary()
{
    if(fixedSharedMemory->allocShmId != mappedAllocationSegment->shmId)
    {
        unmap_allocation_segment();
        map_allocation_segment(fixedSharedMemory->allocShmId);
    }
}

int unmap_allocation_segment()
{
    int shmId = mappedAllocationSegment->shmId;
    unmap_shared_memory(mappedAllocationSegment);
    mappedAllocationSegment = NULL;
    return shmId;
}

void destroy_allocation_segment(int memorySegmentId)
{
    destroy_shared_memory(memorySegmentId);
}

void resize_allocation_segment()
{
    allocation_segment_t *oldMemPtr = mappedAllocationSegment;
    map_allocation_segment(create_allocation_segment(oldMemPtr->bytes * RESIZE_FACTOR));
    copy_blocks_and_append_new_from_free_space_left(oldMemPtr, mappedAllocationSegment);
    fixedSharedMemory->allocShmId = mappedAllocationSegment->shmId;
    int oldMemId = oldMemPtr->shmId;
    unmap_shared_memory(oldMemPtr);
    destroy_allocation_segment(oldMemId);
}

void copy_blocks_and_append_new_from_free_space_left(allocation_segment_t *firstSegment,
                                                     allocation_segment_t *secondSegment)
{
    memcpy(secondSegment + 1, firstSegment + 1, (firstSegment)->bytes);
    secondSegment->blockCount = firstSegment->blockCount + 1;
    secondSegment->usedBlockCount = firstSegment->usedBlockCount;
    secondSegment->firstBlock = firstSegment->firstBlock;
    memory_block_t *oldLastBlock = get_block_from_pointer(firstSegment->lastBlock);
    oldLastBlock->nextBlock = firstSegment->lastBlock + BLOCK_HEADER_SIZE + oldLastBlock->blockSize;
    secondSegment->lastBlock = oldLastBlock->nextBlock;
    secondSegment->head = firstSegment->head;
    memory_block_t *newLastBlock = get_block_from_pointer(secondSegment->lastBlock);
    newLastBlock->isFree = true;
    newLastBlock->nextBlock = 0;
    newLastBlock->prevBlock = firstSegment->lastBlock;
    newLastBlock->blockSize = secondSegment->bytes - firstSegment->bytes - BLOCK_HEADER_SIZE;
}

size_t balloc(size_t bytes)
{
    acquire_semaphore(fixedSharedMemory->semId);
    remap_allocation_segment_if_necessary();
    if(!bytes)
        return 0;
    size_t freeBlockPtr = 0;
    // for now just re-scan the list
    while(is_pointer_null(freeBlockPtr = find_first_fit_free_block(bytes)))
        resize_allocation_segment();
    memory_block_t *freeBlock = get_block_from_pointer(freeBlockPtr);
    freeBlock->isFree = false;
    size_t unusedBytes = freeBlock->blockSize - bytes;
    if(unusedBytes >= BLOCK_HEADER_SIZE + MINIMAL_MEMORY_BLOCK_SIZE)
        make_new_block_with_unused_space(freeBlockPtr, bytes, unusedBytes);
    ++(mappedAllocationSegment->usedBlockCount);
    release_semaphore(fixedSharedMemory->semId);
    return freeBlockPtr;
}

void make_new_block_with_unused_space(size_t blockPtr, size_t bytes, size_t unusedBytes)
{
    memory_block_t *block = get_block_from_pointer(blockPtr);
    block->blockSize = bytes;
    size_t oldNextBlockPtr = block->nextBlock;
    block->nextBlock = blockPtr + BLOCK_HEADER_SIZE + block->blockSize;
    memory_block_t *next = get_block_from_pointer(block->nextBlock);
    next->isFree = true;
    next->nextBlock = oldNextBlockPtr;
    memory_block_t *oldNext = get_block_from_pointer(oldNextBlockPtr);
    if (oldNext)
        oldNext->prevBlock = block->nextBlock;
    next->prevBlock = blockPtr;
    next->blockSize = unusedBytes - BLOCK_HEADER_SIZE;
    if (mappedAllocationSegment->lastBlock == blockPtr)
        mappedAllocationSegment->lastBlock = block->nextBlock;
    ++(mappedAllocationSegment->blockCount);
}

void bfree(size_t ptr)
{
    acquire_semaphore(fixedSharedMemory->semId);
    remap_allocation_segment_if_necessary();
    if(is_pointer_null(ptr))
        return;
    memory_block_t *block = get_block_from_pointer(ptr);
    block->isFree = true;
    --(mappedAllocationSegment->usedBlockCount);
    merge_with_next_block_if_free(ptr);
    merge_with_previous_block_if_free(ptr);
    release_semaphore(fixedSharedMemory->semId);
}

void merge_with_next_block_if_free(size_t blockPtr)
{
    memory_block_t *block = get_block_from_pointer(blockPtr);
    memory_block_t *next = get_block_from_pointer(block->nextBlock);
    if(next && next->isFree)
    {
        if(mappedAllocationSegment->lastBlock == block->nextBlock)
            mappedAllocationSegment->lastBlock = blockPtr;

        block->nextBlock = next->nextBlock;
        memory_block_t *nextNext = get_block_from_pointer(next->nextBlock);
        if(nextNext)
            nextNext->prevBlock = blockPtr;
        block->blockSize += next->blockSize + BLOCK_HEADER_SIZE;
        --(mappedAllocationSegment->blockCount);
    }
}

void merge_with_previous_block_if_free(size_t blockPtr)
{
    memory_block_t *block = get_block_from_pointer(blockPtr);
    memory_block_t *previous = get_block_from_pointer(block->prevBlock);
    if(previous && previous->isFree)
    {
        if(mappedAllocationSegment->lastBlock == blockPtr)
            mappedAllocationSegment->lastBlock = block->prevBlock;

        previous->nextBlock = block->nextBlock;
        memory_block_t *updatedNext = get_block_from_pointer(block->nextBlock);
        if(updatedNext)
            updatedNext->prevBlock = block->prevBlock;
        previous->blockSize += block->blockSize + BLOCK_HEADER_SIZE;
        --(mappedAllocationSegment->blockCount);
    }
}

size_t find_first_fit_free_block(size_t bytes)
{
    size_t current = mappedAllocationSegment->firstBlock;
    while(!is_pointer_null(current))
    {
        memory_block_t *block = get_block_from_pointer(current);
        if(block->isFree && block->blockSize >= bytes)
            return current;
        current = block->nextBlock;
    }
    return 0;
}

memory_block_t *get_block_from_pointer(size_t ptr)
{
    if(ptr < mappedAllocationSegment->firstBlock)
        return NULL;
    return (memory_block_t *) ((char *) mappedAllocationSegment + ptr);
}

void *dereference_pointer(size_t ptr)
{
    if(is_pointer_null(ptr))
        return NULL;
    return (void *) ((char *) mappedAllocationSegment + ptr + BLOCK_HEADER_SIZE);
}

bool is_pointer_null(size_t ptr)
{
    remap_allocation_segment_if_necessary();
    return (ptr < ALLOCATION_SEGMENT_HEADER_SIZE);
}

void set_start_pointer(size_t ptr)
{
    remap_allocation_segment_if_necessary();
    mappedAllocationSegment->head = ptr;
}

size_t get_start_pointer()
{
    remap_allocation_segment_if_necessary();
    return mappedAllocationSegment->head;
}
