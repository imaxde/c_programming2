cmake_minimum_required(VERSION 3.25)
project(Homework C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(bfs src/09-01/main.c src/09-01/graph.c src/09-01/queue.c)
