#ifndef UXP1A_TUPLE_SPACE_H
#define UXP1A_TUPLE_SPACE_H

#include <stdlib.h>


typedef struct tuple_space_t
{
    size_t hashTablePtr;
} tuple_space_t;

void initialize_tuple_space(size_t tupleSpacePtr, size_t initialBucketsCount);
void destroy_tuple_space(size_t tupleSpacePtr);
void push_tuple(size_t tupleSpacePtr, size_t tuplePtr);
int peek_tuple(size_t tupleSpacePtr, size_t pattern, size_t *out);
int pop_tuple(size_t tupleSpacePtr, size_t pattern, size_t *out);

#endif //UXP1A_TUPLE_SPACE_H
