cmake_minimum_required(VERSION 3.12)
project(fpgacompiler C)

set(CMAKE_C_STANDARD 99)

add_executable(fpgacompiler main.c stretchy_buffer.h commons.c)