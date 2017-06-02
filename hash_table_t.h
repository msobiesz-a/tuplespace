#ifndef UXP1A_TEST_HASH_TABLE_T_H
#define UXP1A_TEST_HASH_TABLE_T_H

#include <stdlib.h>
#include <stdbool.h>

#include "list_t.h"
#include "monitor.h"


typedef struct hash_table_t
{
    size_t bucketsCount;
    size_t size;
    ptr_t bucketsPtr;
} hash_table_t;

int initialize_hash_table(ptr_t hashTablePtr, size_t initialBucketsCount);
void destroy_hash_table(ptr_t hashTablePtr);
int put_into_hash_table(ptr_t hashTablePtr, const ptr_t tuplePtr);
int get_from_hash_table(const ptr_t hashTablePtr, const ptr_t patternPtr, ptr_t *tuplePtr);
bool is_in_hash_table(const ptr_t hashTablePtr, const ptr_t tuplePtr);
int remove_from_hash_table(ptr_t hashTablePtr, const ptr_t patternPtr, ptr_t *tuplePtr);

#endif //UXP1A_TEST_HASH_TABLE_T_H
