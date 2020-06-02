cmake_minimum_required(VERSION 3.9)

project(ConstexprCHIP8
    VERSION 0.1.0
    DESCRIPTION "A compile-time CHIP-8 emulator"
    LANGUAGES CXX)

add_executable(chip8_emu include/chip8/chip8.h src/emulator.cpp)
target_include_directories(chip8_emu PUBLIC include)
target_compile_features(chip8_emu PUBLIC cxx_std_17)
set_target_properties(chip8_emu PROPERTIES CXX_EXTENSIONS OFF)
