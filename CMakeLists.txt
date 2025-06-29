cmake_minimum_required(VERSION 3.30)
project(heap_alloc C)

set(CMAKE_C_STANDARD 11)

add_executable(
        heap_alloc
        main.c
        heap_alloc.c
        subscribe_msg.c
        SingleLinkList.c
        timer_msg.c
)

add_executable(
        single_list
        SingleLinkList.c
)