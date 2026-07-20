cmake_minimum_required(VERSION 4.1)
project(c4_compiler C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "-std=gnu23 -g")

set(SOURCE_FILES
    main.c
    lex.c
    lex.h
    sym_tbl.c
    sym_tbl.h
    arena.c
    arena.h)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})
