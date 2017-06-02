#include <stdio.h>

#include "hash_table_t.h"
#include "shared_allocator.h"
#include "tuple_space.h"
#include "tuple_t.h"


void initialize_tuple_space(ptr_t tupleSpacePtr, size_t initialBucketsCount)
{
    ptr_t hashTablePtr = balloc(sizeof(hash_table_t));
    tuple_space_t *tupleSpace = deref_ptr(tupleSpacePtr);
    tupleSpace->hashTablePtr = hashTablePtr;
    initialize_hash_table(tupleSpace->hashTablePtr, initialBucketsCount);
}

void destroy_tuple_space(ptr_t tupleSpacePtr)
{
    tuple_space_t *tupleSpace = deref_ptr(tupleSpacePtr);
    destroy_hash_table(tupleSpace->hashTablePtr);
    bfree(tupleSpacePtr);
}

int push_tuple(ptr_t tupleSpacePtr, ptr_t tuplePtr)
{
    tuple_space_t *tupleSpace = deref_ptr(tupleSpacePtr);
    return put_into_hash_table(tupleSpace->hashTablePtr, tuplePtr);
}

int peek_tuple(ptr_t tupleSpacePtr, ptr_t pattern, ptr_t *out)
{
    tuple_space_t *tupleSpace = deref_ptr(tupleSpacePtr);
    return get_from_hash_table(tupleSpace->hashTablePtr, pattern, out);
}

int pop_tuple(ptr_t tupleSpacePtr, ptr_t pattern, ptr_t *out)
{
    tuple_space_t *tupleSpace = deref_ptr(tupleSpacePtr);
    int status = remove_from_hash_table(tupleSpace->hashTablePtr, pattern, out);
    if(status == 0) {
        ptr_t remote = *out;
        *out = clone_into_local_memory(remote);
        set_remote_mem_ops(true);
        destroy_tuple(remote);
        set_remote_mem_ops(false);
    }
    return status;
}
















