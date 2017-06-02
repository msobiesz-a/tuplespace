#ifndef UXP1A_TUPLE_SPACE_H
#define UXP1A_TUPLE_SPACE_H

#include <stdlib.h>


typedef struct tuple_space_t
{
    ptr_t hashTablePtr;
} tuple_space_t;

void initialize_tuple_space(ptr_t tupleSpacePtr, size_t initialBucketsCount);
void destroy_tuple_space(ptr_t tupleSpacePtr);
int push_tuple(ptr_t tupleSpacePtr, ptr_t tuplePtr);
int peek_tuple(ptr_t tupleSpacePtr, ptr_t pattern, ptr_t *out);
int pop_tuple(ptr_t tupleSpacePtr, ptr_t pattern, ptr_t *out);

#endif //UXP1A_TUPLE_SPACE_H
