#include <string.h>

#include "shared_allocator.h"
#include "shared_memory.h"


static segment_descriptor_t *memPtr;

void create_memory_segment(size_t initialCapacityInBytes)
{
    if(initialCapacityInBytes < sizeof(block_descriptor_t) + 1)
        initialCapacityInBytes = sizeof(block_descriptor_t) + 1;
    int shmId = create_shared_memory(initialCapacityInBytes + sizeof(segment_descriptor_t));
    segment_descriptor_t *shmPtr = map_shared_memory(shmId);
    memPtr = shmPtr;
    shmPtr->shmId = shmId;
    shmPtr->bytes = initialCapacityInBytes;
    shmPtr->blockCount = 1;
    shmPtr->usedBlockCount = 0;
    shmPtr->firstBlock = sizeof(*shmPtr);
    shmPtr->lastBlock = sizeof(*shmPtr);
    block_descriptor_t *block = get_block_from_pointer(shmPtr->firstBlock);
    block->blockSize = initialCapacityInBytes - sizeof(*block);
    block->isFree = true;
    block->nextBlock = 0;
    block->prevBlock = 0;
}

int get_memory_segment_id()
{
    return memPtr ? memPtr->shmId : -1;
}

void map_memory_segment(int shmId)
{
    segment_descriptor_t *ptr = map_shared_memory(shmId);
    memPtr = ptr;
}

void unmap_memory_segment()
{
    unmap_shared_memory(memPtr);
    memPtr = NULL;
}

void destroy_memory_segment(segment_descriptor_t **shmPtr)
{
    if(shmPtr == NULL)
        shmPtr = &memPtr;
    int shmId = (*shmPtr)->shmId;
    unmap_shared_memory(*shmPtr);
    *shmPtr = NULL;
    destroy_shared_memory(shmId);
}

void resize_memory_segment()
{
    segment_descriptor_t *oldMemPtr = memPtr;
    create_memory_segment(oldMemPtr->bytes * 2);
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
    destroy_memory_segment(&oldMemPtr);
}

size_t balloc(size_t bytes)
{
    segment_descriptor_t *s = memPtr;
    if(!bytes)
        return 0;
    size_t blockPtr = find_first_fit_free_block(bytes);
    if(is_pointer_null(blockPtr))
    {
        resize_memory_segment();
        blockPtr = memPtr->lastBlock;
    }
    block_descriptor_t *block = get_block_from_pointer(blockPtr);
    block->isFree = false;
    size_t unusedBytes = 0;
    if(block->blockSize > bytes + sizeof(*block))
         unusedBytes = block->blockSize - bytes - sizeof(*block);
    block->blockSize = bytes;
    if(unusedBytes > sizeof(*block)) {
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
    if(is_pointer_null(ptr))
        return NULL;
    return (block_descriptor_t *) ((char *) memPtr + ptr);
}

void *dereference_pointer(size_t ptr)
{
    if(is_pointer_null(ptr))
        return NULL;
    return (void *) ((char *) memPtr + ptr + sizeof(block_descriptor_t));
}

bool is_pointer_null(size_t ptr)
{
    return (ptr < memPtr->firstBlock);
}

void set_head(size_t ptr)
{
    memPtr->head = ptr;
}

size_t get_head()
{
    return memPtr->head;
}

void bfree(size_t ptr)
{
    segment_descriptor_t *s = memPtr;
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

