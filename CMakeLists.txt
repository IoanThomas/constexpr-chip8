cmake_minimum_required(VERSION 3.9)

project(ConstexprCHIP8
    VERSION 1.0.0
    DESCRIPTION "A compile-time CHIP-8 emulator"
    LANGUAGES CXX)

add_subdirectory(extern)
add_subdirectory(test)
add_subdirectory(src)
