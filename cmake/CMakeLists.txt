cmake_minimum_required(VERSION 2.8)

project(HELLO)

aux_source_directory(./ DIR_SRCS)

add_subdirectory(MathFunc)

add_executable(hello main.c)

target_link_libraries(hello MathFunctions)
