cmake_minimum_required(VERSION 3.28)
project(mbandizip)

set(CMAKE_CXX_STANDARD 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_executable(mbandizip main.cpp  BS_thread_pool.hpp)
