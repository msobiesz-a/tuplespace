#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "shared_allocator.h"
#include "tuple_t.h"
#include "utils.h"

size_t create_tuple()
{
    size_t tuplePtr = balloc(sizeof(tuple_t));
    tuple_t *tuple = dereference_pointer(tuplePtr);
    tuple->len = 0;
    tuple->iCount = 0;
    tuple->sCount = 0;
    tuple->head = 0;
    return tuplePtr;
}

void destroy_tuple(size_t tuplePtr)
{
    tuple_t *tuple = dereference_pointer(tuplePtr);
    destroy_tuple_elements(tuple->head);
    bfree(tuplePtr);
}

void destroy_tuple_elements(size_t headPtr)
{
    while(!is_pointer_null(headPtr))
    {
        size_t oldPtr = headPtr;
        tuple_element_t *element = dereference_pointer(headPtr);
        bfree(element->data.value);
        headPtr = element->next;
        bfree(oldPtr);
    }
}

void add_integer_to_tuple(size_t tuplePtr, int integer)
{
    size_t elementPtr = create_integer_element(integer);
    add_element_to_tuple(tuplePtr, elementPtr);
}

void add_string_to_tuple(size_t tuplePtr, const char *literal)
{
    size_t elementPtr = create_string_element(literal);
    add_element_to_tuple(tuplePtr, elementPtr);
}

size_t create_integer_element(int value)
{
    size_t elementPtr = balloc(sizeof(tuple_element_t));
    size_t integerPtr = balloc(sizeof(int));
    int *integer = dereference_pointer(integerPtr);
    *integer = value;
    tuple_element_t *element = dereference_pointer(elementPtr);
    element->data.valueType = INTEGER;
    element->data.value = integerPtr;
    element->next = 0;
    return elementPtr;
}

size_t create_string_element(const char *literal)
{
    size_t elementPtr = balloc(sizeof(tuple_element_t));
    size_t stringPtr = balloc(strlen(literal) + 1);
    char *string = dereference_pointer(stringPtr);
    strcpy(string, literal);
    tuple_element_t *element = dereference_pointer(elementPtr);
    element->data.valueType = STRING;
    element->data.value = stringPtr;
    element->next = 0;
    return elementPtr;
}

void add_element_to_tuple(size_t tuplePtr, size_t elementPtr)
{
    if(is_pointer_null(elementPtr))
        return;

    tuple_t *tuple = dereference_pointer(tuplePtr);
    tuple_element_t *element = dereference_pointer(elementPtr);
    element->next = 0;
    ++tuple->len;
    switch(element->data.valueType)
    {
        case INTEGER:
            ++tuple->iCount;
            break;
        case STRING:
            ++tuple->sCount;
            break;
        default:
            assert(false);
    }
    size_t currentPtr = tuple->head;
    tuple_element_t *current = NULL;
    while(!is_pointer_null(currentPtr))
    {
        current = dereference_pointer(currentPtr);
        currentPtr = current->next;
    }
    if(current)
        current->next = elementPtr;
    else
    {
        tuple = dereference_pointer(tuplePtr);
        tuple->head = elementPtr;
    }
}

bool does_tuple_match_pattern(tuple_t *tuple, tuple_pattern_t *pattern)
{
    if(tuple->len != pattern->len)
        return false;
    if(tuple->iCount != pattern->iCount)
        return false;
    if(tuple->sCount != pattern->sCount)
        return false;

    tuple_element_t *tupleElement = dereference_pointer(tuple->head);
    pattern_element_t *patternElement = dereference_pointer(pattern->head);
    while(tupleElement && patternElement)
    {
        if(!does_tuple_element_match_pattern_element(tupleElement, patternElement))
            return false;
        tupleElement = dereference_pointer(tupleElement->next);
        patternElement = dereference_pointer(patternElement->next);
    }
    return !tupleElement && !patternElement;
}

bool do_tuples_match(tuple_t *first, tuple_t *second)
{
    if(first->len != second->len)
        return false;
    if(first->iCount != second->iCount)
        return false;
    if(first->sCount != second->sCount)
        return false;

    tuple_element_t *firstElement = dereference_pointer(first->head);
    tuple_element_t *secondElement = dereference_pointer(second->head);
    while(firstElement && secondElement)
    {
        if(!do_tuple_elements_match(firstElement, secondElement))
            return false;
        firstElement = dereference_pointer(firstElement->next);
        secondElement = dereference_pointer(secondElement->next);
    }
    return !firstElement && !secondElement;
}

bool do_tuple_elements_match(tuple_element_t *first, tuple_element_t *second)
{
    return (first->data.valueType == second->data.valueType)
           && are_elements_equal(first, second);
}

bool does_tuple_element_match_pattern_element(tuple_element_t *tupleElement,
                                              pattern_element_t *patternElement)
{
    if(tupleElement->data.valueType != patternElement->data.valueType)
        return false;

    switch(patternElement->conditionType)
    {
        case ANY:
            return tupleElement->data.valueType == patternElement->data.valueType;
        case EQUAL:
            return are_elements_equal(tupleElement, (tuple_element_t *) patternElement);
        case LESS_THAN:
            return is_element_less_than(tupleElement, (tuple_element_t *) patternElement);
        case LESS_EQUAL:
            return is_element_less_equal(tupleElement, (tuple_element_t *) patternElement);
        case GREATER:
            return is_element_greater_than(tupleElement, (tuple_element_t *) patternElement);
        case GREATER_EQUAL:
            return is_element_greater_equal(tupleElement, (tuple_element_t *) patternElement);
        default:
            assert(false);
    }
}

bool are_elements_equal(tuple_element_t *tupleElement, tuple_element_t *patternElement)
{
    switch(tupleElement->data.valueType)
    {
        case INTEGER:
            return  *((int *) dereference_pointer(tupleElement->data.value))
                    == *((int *) dereference_pointer(patternElement->data.value));
        case STRING:
            return strcmp(dereference_pointer(tupleElement->data.value),
                          dereference_pointer(patternElement->data.value)) == 0;
        default:
            assert(false);
    }
}

bool is_element_less_than(tuple_element_t *tupleElement, tuple_element_t *patternElement)
{
    switch(tupleElement->data.valueType)
    {
        case INTEGER:
            return  *((int *) dereference_pointer(tupleElement->data.value))
                    < *((int *) dereference_pointer(patternElement->data.value));
        case STRING:
            return strcmp(dereference_pointer(tupleElement->data.value),
                          dereference_pointer(patternElement->data.value)) < 0;
        default:
            assert(false);
    }
}

bool is_element_less_equal(tuple_element_t *tupleElement, tuple_element_t *patternElement)
{
    switch(tupleElement->data.valueType)
    {
        case INTEGER:
            return  *((int *) dereference_pointer(tupleElement->data.value))
                    <= *((int *) dereference_pointer(patternElement->data.value));
        case STRING:
            return strcmp(dereference_pointer(tupleElement->data.value),
                          dereference_pointer(patternElement->data.value)) <= 0;
        default:
            assert(false);
    }
}

bool is_element_greater_than(tuple_element_t *tupleElement, tuple_element_t *patternElement)
{
    switch(tupleElement->data.valueType)
    {
        case INTEGER:
            return  *((int *) dereference_pointer(tupleElement->data.value))
                    > *((int *) dereference_pointer(patternElement->data.value));
        case STRING:
            return strcmp(dereference_pointer(tupleElement->data.value),
                          dereference_pointer(patternElement->data.value)) > 0;
        default:
            assert(false);
    }
}

bool is_element_greater_equal(tuple_element_t *tupleElement, tuple_element_t *patternElement)
{
    switch(tupleElement->data.valueType)
    {
        case INTEGER:
            return  *((int *) dereference_pointer(tupleElement->data.value))
                    >= *((int *) dereference_pointer(patternElement->data.value));
        case STRING:
            return strcmp(dereference_pointer(tupleElement->data.value),
                          dereference_pointer(patternElement->data.value)) >= 0;
        default:
            assert(false);
    }
}

unsigned long hash_tuple(const void *tuple)
{
    tuple_t *tempTuple = (tuple_t *) tuple;
    unsigned char *valueTypes = malloc(((tempTuple->iCount * INTEGER)
                                        + (tempTuple->sCount * STRING)) * sizeof(char));
    tuple_element_t *element = dereference_pointer(tempTuple->head);
    while(element)
    {
        if(element->data.valueType == INTEGER)
            strcat((char *) valueTypes, "INTEGER");
        else if(element->data.valueType == STRING)
            strcat((char *) valueTypes, "STRING");
        element = dereference_pointer(element->next);
    }
    unsigned long tupleHash = hash(valueTypes);
    free(valueTypes);
    return tupleHash;
}

void print_tuple(size_t tuplePtr)
{
    printf("%s", "t[");
    tuple_t *tuple = dereference_pointer(tuplePtr);
    size_t elementPtr = tuple->head;
    tuple_element_t *element = 0;
    while(!is_pointer_null(elementPtr))
    {
        print_tuple_element(elementPtr);
        element = dereference_pointer(elementPtr);
        elementPtr = element->next;
        if(!is_pointer_null(elementPtr))
            printf("%s", ", ");
    }
    printf("%s", "]\n");
}

void print_tuple_element(size_t elementPtr)
{
    tuple_element_t *element = dereference_pointer(elementPtr);
    switch(element->data.valueType)
    {
        case INTEGER:
            print_integer_element(elementPtr);
            break;
        case STRING:
            print_string_element(elementPtr);
            break;
        default:
            assert(false);
    }
}

void print_integer_element(size_t elementPtr)
{
    tuple_element_t *element = dereference_pointer(elementPtr);
    int *value = dereference_pointer(element->data.value);
    printf("int: %d", *value);
}


void print_string_element(size_t elementPtr)
{
    tuple_element_t *element = dereference_pointer(elementPtr);
    char *value = (char *) dereference_pointer(element->data.value);
    printf("string: %s", value);
}

size_t create_pattern()
{
    size_t patternPtr = balloc(sizeof(tuple_pattern_t));
    tuple_pattern_t *pattern = dereference_pointer(patternPtr);
    pattern->len = 0;
    pattern->iCount = 0;
    pattern->sCount = 0;
    pattern->head = 0;
    return patternPtr;
}

void destroy_patern(size_t patternPtr)
{
    tuple_pattern_t *pattern = dereference_pointer(patternPtr);
    destroy_pattern_elements(pattern->head);
    bfree(patternPtr);
}

void add_integer_to_pattern(size_t patternPtr, int integer, condition_t conditionType)
{
    size_t elementPtr = create_integer_pattern_element(integer, conditionType);
    add_element_to_pattern(patternPtr, elementPtr);
}

void add_string_to_pattern(size_t patternPtr, const char *literal, condition_t conditionType)
{
    size_t elementPtr = create_string_pattern_element(literal, conditionType);
    add_element_to_pattern(patternPtr, elementPtr);
}

void destroy_pattern_elements(size_t headPtr)
{
    while(!is_pointer_null(headPtr))
    {
        size_t oldPtr = headPtr;
        pattern_element_t *element = dereference_pointer(headPtr);
        bfree(element->data.value);
        headPtr = element->next;
        bfree(oldPtr);
    }
}

size_t create_integer_pattern_element(int value, condition_t conditionType)
{
    size_t elementPtr = balloc(sizeof(pattern_element_t));
    size_t integerPtr = balloc(sizeof(int));
    int *integer = dereference_pointer(integerPtr);
    *integer = value;
    pattern_element_t *element = dereference_pointer(elementPtr);
    element->data.valueType = INTEGER;
    element->data.value = integerPtr;
    element->conditionType = conditionType;
    element->next = 0;
    return elementPtr;
}

size_t create_string_pattern_element(const char *literal, condition_t conditionType)
{
    size_t elementPtr = balloc(sizeof(pattern_element_t));
    size_t stringPtr = balloc(strlen(literal) + 1);
    char *string = dereference_pointer(stringPtr);
    strcpy(string, literal);
    pattern_element_t *element = dereference_pointer(elementPtr);
    element->data.valueType = STRING;
    element->data.value = stringPtr;
    element->conditionType = conditionType;
    element->next = 0;
    return elementPtr;
}

void add_element_to_pattern(size_t patternPtr, size_t elementPtr)
{
    if(is_pointer_null(elementPtr))
        return;
    tuple_pattern_t *pattern = dereference_pointer(patternPtr);
    pattern_element_t *element = dereference_pointer(elementPtr);
    element->next = 0;
    ++pattern->len;
    switch(element->data.valueType)
    {
        case INTEGER:
            ++pattern->iCount;
            break;
        case STRING:
            ++pattern->sCount;
            break;
        default:
            assert(false);
    }
    size_t currentPtr = pattern->head;
    pattern_element_t *current = NULL;
    while(!is_pointer_null(currentPtr))
    {
        current = dereference_pointer(currentPtr);
        currentPtr = current->next;
    }
    if(current)
        current->next = elementPtr;
    else
    {
        pattern = dereference_pointer(patternPtr);
        pattern->head = elementPtr;
    }
}

void print_pattern(size_t patternPtr)
{
    printf("%s", "p[");
    tuple_pattern_t *pattern = dereference_pointer(patternPtr);
    size_t elementPtr = pattern->head;
    pattern_element_t *element = 0;
    while(!is_pointer_null(elementPtr))
    {
        print_pattern_element(elementPtr);
        element = dereference_pointer(elementPtr);
        elementPtr = element->next;
        if(!is_pointer_null(elementPtr))
            printf("%s", ", ");
    }
    printf("%s", "]\n");
}

void print_pattern_element(size_t elementPtr)
{
    pattern_element_t *element = dereference_pointer(elementPtr);
    switch(element->data.valueType)
    {
        case INTEGER:
            print_integer_pattern_element(elementPtr);
            break;
        case STRING:
            print_string_pattern_element(elementPtr);
            break;
        default:
            assert(false);
    }
}

void print_integer_pattern_element(size_t elementPtr)
{
    pattern_element_t *element = dereference_pointer(elementPtr);
    int *value = (int *) dereference_pointer(element->data.value);
    printf("int: %d", *value);
}

void print_string_pattern_element(size_t elementPtr)
{
    pattern_element_t *element = dereference_pointer(elementPtr);
    char *value = (char *) dereference_pointer(element->data.value);
    printf("string: %s", value);
}