cmake_minimum_required(VERSION 3.25)
project(Homework C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(numbers src/11-05/main.c src/11-05/automaton.c)
