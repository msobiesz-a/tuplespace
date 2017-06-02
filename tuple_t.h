#ifndef UXP1A_TUPLE_T_H
#define UXP1A_TUPLE_T_H

#include <stdbool.h>
#include <stdlib.h>


typedef enum value_t
{
    STRING = 7,
    INTEGER = 8
} value_t;

typedef struct tuple_element_data_t
{
    value_t valueType;
    ptr_t value;
} tuple_element_data_t;

typedef struct tuple_element_t
{
    tuple_element_data_t data;
    ptr_t next;
} tuple_element_t;

typedef struct tuple_t
{
    size_t len;
    size_t iCount;
    size_t sCount;
    ptr_t head;
} tuple_t;

typedef enum condition_t
{
    ANY = 0,
    EQUAL = 1,
    LESS_THAN = 2,
    LESS_EQUAL = 3,
    GREATER = 4,
    GREATER_EQUAL = 5
} condition_t;

typedef struct pattern_element_t
{
    tuple_element_data_t data;
    ptr_t next;
    condition_t conditionType;
} pattern_element_t;

typedef struct tuple_pattern_t
{
    size_t len;
    size_t iCount;
    size_t sCount;
    ptr_t head;
} tuple_pattern_t;

void set_remote_mem_ops(bool isSet);

ptr_t create_tuple();
void destroy_tuple(ptr_t tuplePtr);
void destroy_tuple_elements(ptr_t headPtr);
void add_integer_to_tuple(ptr_t tuplePtr, int integer);
void add_string_to_tuple(ptr_t tuplePtr, const char *literal);
ptr_t create_integer_element(int value);
ptr_t create_string_element(const char *literal);
void add_element_to_tuple(ptr_t tuplePtr, ptr_t elementPtr);
bool does_tuple_match_pattern(tuple_t *remote, tuple_pattern_t *local);
bool do_tuples_match(tuple_t *remote, tuple_t *local);
bool do_tuple_elements_match(tuple_element_t *first, tuple_element_t *local);
bool does_tuple_element_match_pattern_element(tuple_element_t *remote,
                                              pattern_element_t *local);
bool are_elements_equal(tuple_element_t *remote, tuple_element_t *local);
bool is_element_less_than(tuple_element_t *remote, tuple_element_t *local);
bool is_element_less_equal(tuple_element_t *remote, tuple_element_t *local);
bool is_element_greater_than(tuple_element_t *remote, tuple_element_t *local);
bool is_element_greater_equal(tuple_element_t *remote, tuple_element_t *local);
unsigned long hash_tuple(ptr_t tuplePtr);
void print_tuple(ptr_t tuplePtr);
void print_tuple_element(ptr_t elementPtr);
void print_integer_element(ptr_t elementPtr);
void print_string_element(ptr_t elementPtr);

ptr_t create_pattern();
void destroy_patern(ptr_t patternPtr);
void add_integer_to_pattern(ptr_t patternPtr, int integer, condition_t conditionType);
void add_string_to_pattern(ptr_t patternPtr, const char *literal, condition_t conditionType);
void destroy_pattern_elements(ptr_t headPtr);
ptr_t create_integer_pattern_element(int value, condition_t conditionType);
ptr_t create_string_pattern_element(const char *literal, condition_t conditionType);
void add_element_to_pattern(ptr_t patternPtr, ptr_t elementPtr);
void print_pattern(ptr_t patternPtr);
void print_pattern_element(ptr_t elementPtr);
void print_integer_pattern_element(ptr_t elementPtr);
void print_string_pattern_element(ptr_t elementPtr);

ptr_t clone_into_shared_memory(ptr_t tuplePtr);
ptr_t clone_into_local_memory(ptr_t tuplePtr);

#endif //UXP1A_TUPLE_T_H
