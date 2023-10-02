cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(Game LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
set(CMAKE_COMPILE_WARNING_AS_ERROR On)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_subdirectory(minesweeper)
add_subdirectory(interface-godot)