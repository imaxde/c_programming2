cmake_minimum_required(VERSION 3.25)
project(Homework C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(benchmark src/06-01/main.c src/06-01/avlTree.c src/06-01/linkedList.c)
