#ifndef UXP1A_TUPLE_SPACE_H
#define UXP1A_TUPLE_SPACE_H

#include <stdlib.h>


typedef enum value_t
{
    STRING = 6,
    INTEGER = 7
} value_t;

typedef struct tuple_element_value_t
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

#endif //UXP1A_TUPLE_SPACE_H
