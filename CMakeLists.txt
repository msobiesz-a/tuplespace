cmake_minimum_required(VERSION 3.7)
project(UXP1A)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        list_t.c
        list_t.h
        shared_allocator.c
        shared_allocator.h
        shared_memory.c
        shared_memory.h
        tuple_space.c
        tuple_space.h)

add_executable(test_allocator test_allocator.c test_allocator.h ${SOURCE_FILES})
add_executable(host host.c host.h ${SOURCE_FILES})
add_executable(guest guest.c guest.h ${SOURCE_FILES})