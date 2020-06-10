cmake_minimum_required(VERSION 3.9)

project(ConstexprCHIP8
    VERSION 0.1.0
    DESCRIPTION "A compile-time CHIP-8 emulator"
    LANGUAGES CXX)

find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --remote --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(extern/SFML)

add_executable(tests test/tests.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(tests PRIVATE
    include
    extern/catch2/single_include)

add_executable(chip8_emu
    include/chip8/chip8.h
    src/emulator.cpp
    src/emulator.h
    src/main.cpp)
target_compile_features(chip8_emu PRIVATE cxx_std_17)
set_target_properties(chip8_emu PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(chip8_emu PRIVATE include)
target_link_libraries(chip8_emu PRIVATE sfml-graphics)
