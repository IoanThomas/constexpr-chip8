add_executable(chip8_emu
    chip8.h
    config_file.cpp
    config_file.h
    emulator.cpp
    emulator.h
    main.cpp)
target_compile_features(chip8_emu PRIVATE cxx_std_17)
set_target_properties(chip8_emu PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(chip8_emu PRIVATE sfml-audio sfml-graphics)
target_include_directories(chip8_emu PRIVATE
    "${PROJECT_SOURCE_DIR}/extern/SFML/include")

add_custom_command(TARGET chip8_emu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/extern/SFML/extlibs/bin/x64/openal32.dll"
        $<TARGET_FILE_DIR:chip8_emu>)
        
add_custom_command(TARGET chip8_emu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/data/window.cfg"
        $<TARGET_FILE_DIR:chip8_emu>)
