cmake_minimum_required(VERSION 3.8)
project(CS530Assembler)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        file_parse_exception.h
        file_parser.cc
        file_parser.h
        opcode_error_exception.h
        opcodetab.cc
        opcodetab.h
        sicxe_asm.cpp
        sicxe_asm.h
        symtab.cc
        symtab.h symtab_exception.h)

add_executable(CS530Assembler ${SOURCE_FILES})