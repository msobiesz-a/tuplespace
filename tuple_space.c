#include <stdio.h>

#include "hash_table_t.h"
#include "shared_allocator.h"
#include "tuple_space.h"


void initialize_tuple_space(size_t tupleSpacePtr, size_t initialBucketsCount)
{
    size_t hashTablePtr = balloc(sizeof(hash_table_t));
    tuple_space_t *tupleSpace = dereference_pointer(tupleSpacePtr);
    tupleSpace->hashTablePtr = hashTablePtr;
    initialize_hash_table(tupleSpace->hashTablePtr, initialBucketsCount);
}

void destroy_tuple_space(size_t tupleSpacePtr)
{
    tuple_space_t *tupleSpace = dereference_pointer(tupleSpacePtr);
    destroy_hash_table(tupleSpace->hashTablePtr);
    bfree(tupleSpacePtr);
}

int push_tuple(size_t tupleSpacePtr, size_t tuplePtr)
{
    tuple_space_t *tupleSpace = dereference_pointer(tupleSpacePtr);
    return put_into_hash_table(tupleSpace->hashTablePtr, tuplePtr);
}

int peek_tuple(size_t tupleSpacePtr, size_t pattern, size_t *out)
{
    tuple_space_t *tupleSpace = dereference_pointer(tupleSpacePtr);
    return get_from_hash_table(tupleSpace->hashTablePtr, pattern, out);
}

int pop_tuple(size_t tupleSpacePtr, size_t pattern, size_t *out)
{
    tuple_space_t *tupleSpace = dereference_pointer(tupleSpacePtr);
    return remove_from_hash_table(tupleSpace->hashTablePtr, pattern, out);
}
















