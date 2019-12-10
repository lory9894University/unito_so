cmake_minimum_required(VERSION 3.15)
project(SO_project C)

set(CMAKE_C_STANDARD 90)

add_executable(SO_project master.c env_operation.h error_handling.h player.h pawn.h player.c shared_res.h pawn.c shared_res.c)