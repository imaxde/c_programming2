cmake_minimum_required(VERSION 3.25)
project(Homework C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(aerosoft src/06-03/main.c src/06-03/avlTree.c)
