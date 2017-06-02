#ifndef UXP1A_TEST_UTILS_H
#define UXP1A_TEST_UTILS_H

#include <stdbool.h>
#include <stdint.h>

typedef uintptr_t ptr_t;

typedef struct mem_ops
{
    ptr_t (*alloc)(ptr_t bytes);
    void (*dealloc)(ptr_t ptr);
    void *(*deref)(ptr_t ptr);
    bool (*is_null)(ptr_t ptr);
} mem_ops;

unsigned long hash(unsigned char *str);
void write_handle_to_file(const char *filename, int shmId);
int read_handle_from_file(const char *filename);
ptr_t malloc_l(ptr_t bytes);
void free_l(ptr_t ptr);
void *deref_l(ptr_t ptr);
bool is_null_l(ptr_t ptr);

#endif //UXP1A_TEST_UTILS_H
