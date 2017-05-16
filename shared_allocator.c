#include <printf.h>
#include <stdio.h>
#include <string.h>


#include "shared_allocator.h"
#include "shared_memory.h"


const size_t INITIAL_CAPACITY = 32;

static fixed_memory_t *fixedPtr;
static segment_descriptor_t *memPtr;

int create_fixed_memory()
{
    int shmId = create_shared_memory(sizeof(fixed_memory_t));
    fixed_memory_t *shmPtr = map_shared_memory(shmId);
    shmPtr->memShmId = create_memory_segment(INITIAL_CAPACITY);
    unmap_shared_memory(shmPtr);
    return shmId;
}

void map_fixed_memory(int shmId)
{
    fixedPtr = map_shared_memory(shmId);
    map_memory_segment(fixedPtr->memShmId);
}

void unmap_fixed_memory()
{
    unmap_memory_segment();
    unmap_shared_memory(fixedPtr);
    fixedPtr = NULL;
}

void destroy_fixed_memory(int fixedMemoryId)
{
    map_fixed_memory(fixedMemoryId);
    int memShmId = fixedPtr->memShmId;
    unmap_fixed_memory();
    destroy_memory_segment(memShmId);
    destroy_shared_memory(fixedMemoryId);
}

int create_memory_segment(size_t initialCapacityInBytes)
{
    if(initialCapacityInBytes < sizeof(block_descriptor_t) + 1)
        initialCapacityInBytes = sizeof(block_descriptor_t) + 1;
    int shmId = create_shared_memory(initialCapacityInBytes + sizeof(segment_descriptor_t));
    map_memory_segment(shmId);

    memPtr->shmId = shmId;
    memPtr->bytes = initialCapacityInBytes;
    memPtr->blockCount = 1;
    memPtr->usedBlockCount = 0;
    memPtr->firstBlock = sizeof(*memPtr);
    memPtr->lastBlock = sizeof(*memPtr);
    memPtr->head = 0;
    block_descriptor_t *firstBlock = get_block_from_pointer(memPtr->firstBlock);
    firstBlock->blockSize = initialCapacityInBytes - sizeof(*firstBlock);
    firstBlock->isFree = true;
    firstBlock->nextBlock = 0;
    firstBlock->prevBlock = 0;

    unmap_memory_segment();
    return shmId;
}

void map_memory_segment(int shmId)
{
    memPtr = map_shared_memory(shmId);
}

void remap_memory_segment_if_needed()
{
    if(fixedPtr->memShmId != memPtr->shmId)
    {
        unmap_memory_segment();
        map_memory_segment(fixedPtr->memShmId);
    }
}

int unmap_memory_segment()
{
    int shmId = memPtr->shmId;
    unmap_shared_memory(memPtr);
    memPtr = NULL;

    return shmId;
}

void destroy_memory_segment(int memorySegmentId)
{

    destroy_shared_memory(memorySegmentId);
}

void resize_memory_segment()
{
    segment_descriptor_t *oldMemPtr = memPtr;
    int shmId = create_memory_segment(oldMemPtr->bytes * 2);
    map_memory_segment(shmId);
    memcpy(memPtr + 1, oldMemPtr + 1, (oldMemPtr)->bytes);
    memPtr->blockCount = oldMemPtr->blockCount + 1;
    memPtr->usedBlockCount = oldMemPtr->usedBlockCount;
    memPtr->firstBlock = oldMemPtr->firstBlock;
    block_descriptor_t *block = get_block_from_pointer(oldMemPtr->lastBlock);
    block->nextBlock = oldMemPtr->lastBlock + sizeof(*block) + block->blockSize;
    memPtr->lastBlock = block->nextBlock;
    memPtr->head = oldMemPtr->head;
    block_descriptor_t *newBlock = get_block_from_pointer(memPtr->lastBlock);
    newBlock->isFree = true;
    newBlock->nextBlock = 0;
    newBlock->prevBlock = oldMemPtr->lastBlock;
    newBlock->blockSize = memPtr->bytes - oldMemPtr->bytes - sizeof(*newBlock);
    fixedPtr->memShmId = memPtr->shmId;
    int oldMemId = oldMemPtr->shmId;
    unmap_shared_memory(oldMemPtr);
    destroy_memory_segment(oldMemId);
}

size_t balloc(size_t bytes)
{
    remap_memory_segment_if_needed();
    if(!bytes)
        return 0;
    size_t blockPtr = 0;
    // for now just re-scan the list
    while(is_pointer_null(blockPtr = find_first_fit_free_block(bytes)))
        resize_memory_segment();
    block_descriptor_t *block = get_block_from_pointer(blockPtr);
    block->isFree = false;
    size_t unusedBytes = 0;
    if(block->blockSize > bytes + sizeof(*block))
         unusedBytes = block->blockSize - bytes - sizeof(*block);
    if(unusedBytes > sizeof(*block)) {
        block->blockSize = bytes;
        size_t oldNextPtr = block->nextBlock;
        block->nextBlock = blockPtr + sizeof(*block) + block->blockSize;
        block_descriptor_t *next = get_block_from_pointer(block->nextBlock);
        next->isFree = true;
        next->nextBlock = oldNextPtr;
        block_descriptor_t *oldNext = get_block_from_pointer(oldNextPtr);
        if (oldNext)
            oldNext->prevBlock = block->nextBlock;
        next->prevBlock = blockPtr;
        next->blockSize = unusedBytes;
        if (memPtr->lastBlock == blockPtr)
            memPtr->lastBlock = block->nextBlock;
        ++(memPtr->blockCount);
    }
    block->blockSize = bytes + unusedBytes;
    ++(memPtr->usedBlockCount);
    return blockPtr;
}

size_t find_first_fit_free_block(size_t bytes)
{
    size_t current = memPtr->firstBlock;
    while(!is_pointer_null(current))
    {
        block_descriptor_t *block = get_block_from_pointer(current);
        if(block->isFree && block->blockSize >= bytes)
            return current;
        current = block->nextBlock;
    }
    return 0;
}

block_descriptor_t *get_block_from_pointer(size_t ptr)
{
    if(ptr < memPtr->firstBlock)
        return NULL;
    return (block_descriptor_t *) ((char *) memPtr + ptr);
}

void *dereference_pointer(size_t ptr)
{
    if(is_pointer_null(ptr))
        return NULL;
    remap_memory_segment_if_needed();
    return (void *) ((char *) memPtr + ptr + sizeof(block_descriptor_t));
}

bool is_pointer_null(size_t ptr)
{
    remap_memory_segment_if_needed();
    return (ptr < memPtr->firstBlock);
}

void set_head(size_t ptr)
{
    remap_memory_segment_if_needed();
    memPtr->head = ptr;
}

size_t get_head()
{
    remap_memory_segment_if_needed();
    return memPtr->head;
}

void bfree(size_t ptr)
{
    // heap corruption protection, e.g. double bfree call, see corresponding note
    remap_memory_segment_if_needed();
    if(is_pointer_null(ptr))
        return;
    block_descriptor_t *block = get_block_from_pointer(ptr);
    block->isFree = true;
    --(memPtr->usedBlockCount);

    block_descriptor_t *next = get_block_from_pointer(block->nextBlock);
    if(next && next->isFree)
    {
        if(memPtr->lastBlock == block->nextBlock)
            memPtr->lastBlock = ptr;

        block->nextBlock = next->nextBlock;
        block_descriptor_t *nextNext = get_block_from_pointer(next->nextBlock);
        if(nextNext)
            nextNext->prevBlock = ptr;
        block->blockSize += next->blockSize;
        --(memPtr->blockCount);
    }

    block_descriptor_t *previous = get_block_from_pointer(block->prevBlock);
    if(previous && previous->isFree)
    {
        if(memPtr->lastBlock == ptr)
            memPtr->lastBlock = block->prevBlock;
        previous->nextBlock = block->nextBlock;
        if(next)
            next->prevBlock = block->prevBlock;
        previous->blockSize += block->blockSize;
        --(memPtr->blockCount);
    }
}

void print_memory_info()
{
    if(memPtr == NULL)
        return;
    printf("[Blocks used: %zu/%zu. Total bytes: %zu]\n",
           memPtr->usedBlockCount,
           memPtr->blockCount,
           memPtr->bytes);
}
