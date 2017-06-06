#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "shared_allocator.h"
#include "tuple_t.h"
#include "utils.h"


static mem_ops mO;

void set_remote_mem_ops(bool isSet)
{
    isSet ? (mO.alloc = balloc) : (mO.alloc = malloc_l);
    isSet ? (mO.dealloc = bfree) : (mO.dealloc = free_l);
    isSet ? (mO.deref = deref_ptr) : (mO.deref = deref_l);
    isSet ? (mO.is_null = is_ptr_null) : (mO.is_null = is_null_l);
}

ptr_t create_tuple()
{
    ptr_t tuplePtr = mO.alloc(sizeof(tuple_t));
    tuple_t *tuple = mO.deref(tuplePtr);
    tuple->len = 0;
    tuple->iCount = 0;
    tuple->sCount = 0;
    tuple->head = 0;
    return tuplePtr;
}

void destroy_tuple(ptr_t tuplePtr)
{
    tuple_t *tuple = mO.deref(tuplePtr);
    destroy_tuple_elements(tuple->head);
    mO.dealloc(tuplePtr);
}

void destroy_tuple_elements(ptr_t headPtr)
{
    while(!is_null_l(headPtr))
    {
        ptr_t oldPtr = headPtr;
        tuple_element_t *element = mO.deref(headPtr);
        mO.dealloc(element->data.value);
        headPtr = element->next;
        mO.dealloc(oldPtr);
    }
}

void add_integer_to_tuple(ptr_t tuplePtr, int integer)
{
    ptr_t elementPtr = create_integer_element(integer);
    add_element_to_tuple(tuplePtr, elementPtr);
}

void add_string_to_tuple(ptr_t tuplePtr, const char *literal)
{
    ptr_t elementPtr = create_string_element(literal);
    add_element_to_tuple(tuplePtr, elementPtr);
}

ptr_t create_integer_element(int value)
{
    ptr_t elementPtr = mO.alloc(sizeof(tuple_element_t));
    ptr_t integerPtr = mO.alloc(sizeof(int));
    int *integer = mO.deref(integerPtr);
    *integer = value;
    tuple_element_t *element = mO.deref(elementPtr);
    element->data.valueType = INTEGER;
    element->data.value = integerPtr;
    element->next = 0;
    return elementPtr;
}

ptr_t create_string_element(const char *literal)
{
    ptr_t elementPtr = mO.alloc(sizeof(tuple_element_t));
    ptr_t stringPtr = mO.alloc(strlen(literal) + 1);
    char *string = mO.deref(stringPtr);
    strcpy(string, literal);
    tuple_element_t *element = mO.deref(elementPtr);
    element->data.valueType = STRING;
    element->data.value = stringPtr;
    element->next = 0;
    return elementPtr;
}

void add_element_to_tuple(ptr_t tuplePtr, ptr_t elementPtr)
{
    if(is_null_l(elementPtr))
        return;

    tuple_t *tuple = mO.deref(tuplePtr);
    tuple_element_t *element = mO.deref(elementPtr);
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
    ptr_t currentPtr = tuple->head;
    tuple_element_t *current = NULL;
    while(!is_null_l(currentPtr))
    {
        current = mO.deref(currentPtr);
        currentPtr = current->next;
    }
    if(current)
        current->next = elementPtr;
    else
    {
        tuple = mO.deref(tuplePtr);
        tuple->head = elementPtr;
    }
}

bool does_tuple_match_pattern(tuple_t *remote, tuple_pattern_t *local)
{
    if(remote->len != local->len)
        return false;
    if(remote->iCount != local->iCount)
        return false;
    if(remote->sCount != local->sCount)
        return false;

    tuple_element_t *tupleElement = deref_ptr(remote->head);
    pattern_element_t *patternElement = deref_l(local->head);
    while(tupleElement && patternElement)
    {
        if(!does_tuple_element_match_pattern_element(tupleElement, patternElement))
            return false;
        tupleElement = deref_ptr(tupleElement->next);
        patternElement = deref_l(patternElement->next);
    }
    return !tupleElement && !patternElement;
}

bool do_tuples_match(tuple_t *remote, tuple_t *local)
{
    if(remote->len != local->len)
        return false;
    if(remote->iCount != local->iCount)
        return false;
    if(remote->sCount != local->sCount)
        return false;

    tuple_element_t *remoteElement = deref_ptr(remote->head);
    tuple_element_t *localElement = deref_l(local->head);
    while(remoteElement && localElement)
    {
        if(!do_tuple_elements_match(remoteElement, localElement))
            return false;
        remoteElement = deref_ptr(remoteElement->next);
        localElement = deref_l(localElement->next);
    }
    return !remoteElement && !localElement;
}

bool do_tuple_elements_match(tuple_element_t *remote, tuple_element_t *local)
{
    return (remote->data.valueType == local->data.valueType)
           && are_elements_equal(remote, local);
}

bool does_tuple_element_match_pattern_element(tuple_element_t *remote,
                                              pattern_element_t *local)
{
    if(remote->data.valueType != local->data.valueType)
        return false;

    switch(local->conditionType)
    {
        case ANY:
            return remote->data.valueType == local->data.valueType;
        case EQUAL:
            return are_elements_equal(remote, (tuple_element_t *) local);
        case LESS_THAN:
            return is_element_less_than(remote, (tuple_element_t *) local);
        case LESS_EQUAL:
            return is_element_less_equal(remote, (tuple_element_t *) local);
        case GREATER:
            return is_element_greater_than(remote, (tuple_element_t *) local);
        case GREATER_EQUAL:
            return is_element_greater_equal(remote, (tuple_element_t *) local);
        default:
            assert(false);
    }
}

bool are_elements_equal(tuple_element_t *remote, tuple_element_t *local)
{
    switch(remote->data.valueType)
    {
        case INTEGER:
            return  *((int *) deref_ptr(remote->data.value))
                    == *((int *) deref_l(local->data.value));
        case STRING:
            return strcmp(deref_ptr(remote->data.value),
                          deref_l(local->data.value)) == 0;
        default:
            assert(false);
    }
}

bool is_element_less_than(tuple_element_t *remote, tuple_element_t *local)
{
    switch(remote->data.valueType)
    {
        case INTEGER:
            return  *((int *) deref_ptr(remote->data.value))
                    < *((int *) deref_l(local->data.value));
        case STRING:
            return strcmp(deref_ptr(remote->data.value),
                          deref_l(local->data.value)) < 0;
        default:
            assert(false);
    }
}

bool is_element_less_equal(tuple_element_t *remote, tuple_element_t *local)
{
    switch(remote->data.valueType)
    {
        case INTEGER:
            return  *((int *) deref_ptr(remote->data.value))
                    <= *((int *) deref_l(local->data.value));
        case STRING:
            return strcmp(deref_ptr(remote->data.value),
                          deref_l(local->data.value)) <= 0;
        default:
            assert(false);
    }
}

bool is_element_greater_than(tuple_element_t *remote, tuple_element_t *local)
{
    switch(remote->data.valueType)
    {
        case INTEGER:
            return  *((int *) deref_ptr(remote->data.value))
                    > *((int *) deref_l(local->data.value));
        case STRING:
            return strcmp(deref_ptr(remote->data.value),
                          deref_l(local->data.value)) > 0;
        default:
            assert(false);
    }
}

bool is_element_greater_equal(tuple_element_t *remote, tuple_element_t *local)
{
    switch(remote->data.valueType)
    {
        case INTEGER:
            return  *((int *) deref_ptr(remote->data.value))
                    >= *((int *) deref_l(local->data.value));
        case STRING:
            return strcmp(deref_ptr(remote->data.value),
                          deref_l(local->data.value)) >= 0;
        default:
            assert(false);
    }
}

unsigned long hash_tuple(ptr_t tuplePtr)
{
    tuple_t *tuple = mO.deref(tuplePtr);
    char *valueTypes = calloc(((tuple->iCount * INTEGER)
                                        + (tuple->sCount * STRING)), sizeof(char));
    ptr_t elementPtr = tuple->head;
    tuple_element_t *element = mO.deref(tuple->head);
    while(!is_null_l(elementPtr))
    {
        if(element->data.valueType == INTEGER)
            strcat(valueTypes, "INTEGER");
        else if(element->data.valueType == STRING)
            strcat(valueTypes, "STRING");
        else
            assert(false);
        elementPtr = element->next;
        element = mO.deref(elementPtr);
    }

    unsigned long tupleHash = hash((unsigned char *) valueTypes);
    free(valueTypes);
    return tupleHash;
}

void print_tuple(ptr_t tuplePtr)
{
    printf("%s", "t[");
    tuple_t *tuple = mO.deref(tuplePtr);
    ptr_t elementPtr = tuple->head;
    tuple_element_t *element = 0;
    while(!is_null_l(elementPtr))
    {
        print_tuple_element(elementPtr);
        element = mO.deref(elementPtr);
        elementPtr = element->next;
        if(!is_null_l(elementPtr))
            printf("%s", ", ");
    }
    printf("%s", "]\n");
}

void print_tuple_element(ptr_t elementPtr)
{
    tuple_element_t *element = mO.deref(elementPtr);
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

void print_integer_element(ptr_t elementPtr)
{
    tuple_element_t *element = mO.deref(elementPtr);
    int *value = mO.deref(element->data.value);
    printf("int: %d", *value);
}


void print_string_element(ptr_t elementPtr)
{
    tuple_element_t *element = mO.deref(elementPtr);
    char *value = (char *) mO.deref(element->data.value);
    printf("string: %s", value);
}

ptr_t create_pattern()
{
    ptr_t patternPtr = mO.alloc(sizeof(tuple_pattern_t));
    tuple_pattern_t *pattern = mO.deref(patternPtr);
    pattern->len = 0;
    pattern->iCount = 0;
    pattern->sCount = 0;
    pattern->head = 0;
    return patternPtr;
}

void destroy_patern(ptr_t patternPtr)
{
    tuple_pattern_t *pattern = mO.deref(patternPtr);
    destroy_pattern_elements(pattern->head);
    mO.dealloc(patternPtr);
}

void add_integer_to_pattern(ptr_t patternPtr, int integer, condition_t conditionType)
{
    ptr_t elementPtr = create_integer_pattern_element(integer, conditionType);
    add_element_to_pattern(patternPtr, elementPtr);
}

void add_string_to_pattern(ptr_t patternPtr, const char *literal, condition_t conditionType)
{
    ptr_t elementPtr = create_string_pattern_element(literal, conditionType);
    add_element_to_pattern(patternPtr, elementPtr);
}

void destroy_pattern_elements(ptr_t headPtr)
{
    while(!is_null_l(headPtr))
    {
        ptr_t oldPtr = headPtr;
        pattern_element_t *element = mO.deref(headPtr);
        mO.dealloc(element->data.value);
        headPtr = element->next;
        mO.dealloc(oldPtr);
    }
}

ptr_t create_integer_pattern_element(int value, condition_t conditionType)
{
    ptr_t elementPtr = mO.alloc(sizeof(pattern_element_t));
    ptr_t integerPtr = mO.alloc(sizeof(int));
    int *integer = mO.deref(integerPtr);
    *integer = value;
    pattern_element_t *element = mO.deref(elementPtr);
    element->data.valueType = INTEGER;
    element->data.value = integerPtr;
    element->conditionType = conditionType;
    element->next = 0;
    return elementPtr;
}

ptr_t create_string_pattern_element(const char *literal, condition_t conditionType)
{
    ptr_t elementPtr = mO.alloc(sizeof(pattern_element_t));
    ptr_t stringPtr = mO.alloc(strlen(literal) + 1);
    char *string = mO.deref(stringPtr);
    strcpy(string, literal);
    pattern_element_t *element = mO.deref(elementPtr);
    element->data.valueType = STRING;
    element->data.value = stringPtr;
    element->conditionType = conditionType;
    element->next = 0;
    return elementPtr;
}

void add_element_to_pattern(ptr_t patternPtr, ptr_t elementPtr)
{
    if(is_null_l(elementPtr))
        return;
    tuple_pattern_t *pattern = mO.deref(patternPtr);
    pattern_element_t *element = mO.deref(elementPtr);
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
    ptr_t currentPtr = pattern->head;
    pattern_element_t *current = NULL;
    while(!is_null_l(currentPtr))
    {
        current = mO.deref(currentPtr);
        currentPtr = current->next;
    }
    if(current)
        current->next = elementPtr;
    else
    {
        pattern = mO.deref(patternPtr);
        pattern->head = elementPtr;
    }
}

void print_pattern(ptr_t patternPtr)
{
    printf("%s", "p[");
    tuple_pattern_t *pattern = mO.deref(patternPtr);
    ptr_t elementPtr = pattern->head;
    pattern_element_t *element = 0;
    while(!is_null_l(elementPtr))
    {
        print_pattern_element(elementPtr);
        element = mO.deref(elementPtr);
        elementPtr = element->next;
        if(!is_null_l(elementPtr))
            printf("%s", ", ");
    }
    printf("%s", "]\n");
}

void print_pattern_element(ptr_t elementPtr)
{
    pattern_element_t *element = mO.deref(elementPtr);
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

void print_integer_pattern_element(ptr_t elementPtr)
{
    pattern_element_t *element = mO.deref(elementPtr);
    int *value = (int *) mO.deref(element->data.value);
    printf("int: %d", *value);
}

void print_string_pattern_element(ptr_t elementPtr)
{
    pattern_element_t *element = mO.deref(elementPtr);
    char *value = (char *) mO.deref(element->data.value);
    printf("string: %s", value);
}

ptr_t clone_into_shared_memory(ptr_t tuplePtr)
{
    ptr_t copyTuplePtr = balloc(sizeof(tuple_t));
    tuple_t *tuple = deref_l(tuplePtr);
    tuple_t *copyTuple = deref_ptr(copyTuplePtr);
    copyTuple->len = tuple->len;
    copyTuple->iCount = tuple->iCount;
    copyTuple->sCount = tuple->sCount;
    copyTuple->head = 0;

    ptr_t prevCopyElemPtr = 0;
    ptr_t currElemPtr = tuple->head;
    tuple_element_t *prevCopyElem = NULL;

    while(!is_null_l(currElemPtr))
    {
        ptr_t copyElementPtr = balloc(sizeof(tuple_element_t));
        copyTuple = deref_ptr(copyTuplePtr);
        if(is_ptr_null(copyTuple->head))
            copyTuple->head = copyElementPtr;
        if(!is_ptr_null(prevCopyElemPtr))
        {
            prevCopyElem = deref_ptr(prevCopyElemPtr);
            prevCopyElem->next = copyElementPtr;
        }
        tuple_element_t *copyElement = deref_ptr(copyElementPtr);
        tuple_element_t *element = deref_l(currElemPtr);
        ptr_t valuePtr = 0;
        switch(element->data.valueType)
        {
            case INTEGER:
                valuePtr = balloc(sizeof(int));
                int *integer = deref_ptr(valuePtr);
                element = deref_l(currElemPtr);
                *integer = *((int *) deref_l(element->data.value));
                copyElement = deref_ptr(copyElementPtr);
                copyElement->data.valueType = INTEGER;
                break;
            case STRING:
                valuePtr = balloc(strlen(deref_l(element->data.value)) + 1);
                char *string = deref_ptr(valuePtr);
                element = deref_l(currElemPtr);
                strcpy(string, deref_l(element->data.value));
                copyElement = deref_ptr(copyElementPtr);
                copyElement->data.valueType = STRING;
                break;
            default:
                assert(false);
        }
        copyElement = deref_ptr(copyElementPtr);
        copyElement->data.value = valuePtr;
        copyElement->next = 0;
        prevCopyElemPtr = copyElementPtr;
        element = deref_l(currElemPtr);
        currElemPtr = element->next;
    }

    return copyTuplePtr;
}

ptr_t clone_into_local_memory(ptr_t tuplePtr)
{
    ptr_t copyTuplePtr = malloc_l(sizeof(tuple_t));
    tuple_t *tuple = deref_ptr(tuplePtr);
    tuple_t *copyTuple = deref_l(copyTuplePtr);
    copyTuple->len = tuple->len;
    copyTuple->iCount = tuple->iCount;
    copyTuple->sCount = tuple->sCount;
    copyTuple->head = 0;

    ptr_t prevCopyElemPtr = 0;
    ptr_t currElemPtr = tuple->head;
    tuple_element_t *prevCopyElem = NULL;

    while(!is_ptr_null(currElemPtr))
    {
        ptr_t copyElementPtr = malloc_l(sizeof(tuple_element_t));
        copyTuple = deref_l(copyTuplePtr);
        if(is_null_l(copyTuple->head))
            copyTuple->head = copyElementPtr;
        if(!is_null_l(prevCopyElemPtr))
        {
            prevCopyElem = deref_l(prevCopyElemPtr);
            prevCopyElem->next = copyElementPtr;
        }
        tuple_element_t *copyElement = deref_l(copyElementPtr);
        tuple_element_t *element = deref_ptr(currElemPtr);
        ptr_t valuePtr = 0;
        switch(element->data.valueType)
        {
            case INTEGER:
                valuePtr = malloc_l(sizeof(int));
                int *integer = deref_l(valuePtr);
                element = deref_ptr(currElemPtr);
                *integer = *((int *) deref_ptr(element->data.value));
                copyElement = deref_l(copyElementPtr);
                copyElement->data.valueType = INTEGER;
                break;
            case STRING:
                valuePtr = malloc_l(strlen(deref_ptr(element->data.value)) + 1);
                char *string = deref_l(valuePtr);
                element = deref_ptr(currElemPtr);
                strcpy(string, deref_ptr(element->data.value));
                copyElement = deref_l(copyElementPtr);
                copyElement->data.valueType = STRING;
                break;
            default:
                assert(false);
        }
        copyElement = deref_l(copyElementPtr);
        copyElement->data.value = valuePtr;
        copyElement->next = 0;
        prevCopyElemPtr = copyElementPtr;
        element = deref_ptr(currElemPtr);
        currElemPtr = element->next;
    }
    return copyTuplePtr;
}