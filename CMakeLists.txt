cmake_minimum_required(VERSION 3.7)
project(tuplespace)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        hash_table_t.c
        hash_table_t.h
        list_t.c
        list_t.h
        monitor.c
        monitor.h
        shared_allocator.c
        shared_allocator.h
        shared_memory.c
        shared_memory.h
        tuple_space.c
        tuple_space.h
        utils.c
        utils.h
        tuple_t.c
        tuple_t.h
        )

add_executable(host host.c ${SOURCE_FILES})
add_executable(guest guest.c ${SOURCE_FILES})
add_executable(guest1 guest1.c ${SOURCE_FILES})
add_executable(guest2 guest2.c ${SOURCE_FILES})
add_executable(guest3 guest3.c ${SOURCE_FILES})
add_executable(testInteger testInt.c ${SOURCE_FILES})
add_executable(testString testString.c ${SOURCE_FILES})
add_executable(testSynchronization testSynchronization.c ${SOURCE_FILES})