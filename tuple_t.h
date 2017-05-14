#ifndef UXP1A_TUPLE_T_H
#define UXP1A_TUPLE_T_H

#include <stdbool.h>
#include <stdlib.h>


typedef enum value_t
{
    STRING = 6,
    INTEGER = 7
} value_t;

typedef struct tuple_element_data_t
{
    value_t valueType;
    size_t value;
} tuple_element_data_t;

typedef struct tuple_element_t
{
    tuple_element_data_t data;
    size_t next;
} tuple_element_t;

typedef struct tuple_t
{
    size_t len;
    size_t iCount;
    size_t sCount;
    size_t head;
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
    size_t next;
    condition_t conditionType;
} pattern_element_t;

typedef struct tuple_pattern_t
{
    size_t len;
    size_t iCount;
    size_t sCount;
    size_t head;
} tuple_pattern_t;

size_t create_tuple();
void destroy_tuple(size_t tuplePtr);
void destroy_tuple_elements(size_t headPtr);
void add_integer_to_tuple(size_t tuplePtr, int integer);
void add_string_to_tuple(size_t tuplePtr, const char *literal);
size_t create_integer_element(int value);
size_t create_string_element(const char *literal);
void add_element_to_tuple(size_t tuplePtr, size_t elementPtr);
bool match_tuple_to_pattern(const void *tuple, const void *pattern);
bool match_tuples(const void *first, const void *second);
bool does_tuple_match_pattern(tuple_t *tuple, tuple_pattern_t *pattern);
bool do_tuples_match(tuple_t *first, tuple_t *second);
bool do_tuple_elements_match(tuple_element_t *first, tuple_element_t *second);
bool does_tuple_element_match_pattern_element(tuple_element_t *tupleElement,
                                              pattern_element_t *patternElement);
bool are_elements_equal(tuple_element_t *tupleElement, tuple_element_t *patternElement);
bool is_element_less_than(tuple_element_t *tupleElement, tuple_element_t *patternElement);
bool is_element_less_equal(tuple_element_t *tupleElement, tuple_element_t *patternElement);
bool is_element_greater_than(tuple_element_t *tupleElement, tuple_element_t *patternElement);
bool is_element_greater_equal(tuple_element_t *tupleElement, tuple_element_t *patternElement);
unsigned long hash_tuple(const void *tuple);
void print_tuple(size_t tuplePtr);
void print_tuple_element(size_t elementPtr);
void print_integer_element(size_t elementPtr);
void print_string_element(size_t elementPtr);

size_t create_pattern();
void destroy_patern(size_t patternPtr);
void add_integer_to_pattern(size_t patternPtr, int integer, condition_t conditionType);
void add_string_to_pattern(size_t patternPtr, const char *literal, condition_t conditionType);
void destroy_pattern_elements(size_t headPtr);
size_t create_integer_pattern_element(int value, condition_t conditionType);
size_t create_string_pattern_element(const char *literal, condition_t conditionType);
void add_element_to_pattern(size_t patternPtr, size_t elementPtr);
void print_pattern(size_t patternPtr);
void print_pattern_element(size_t elementPtr);
void print_integer_pattern_element(size_t elementPtr);
void print_string_pattern_element(size_t elementPtr);
#endif //UXP1A_TUPLE_T_H
