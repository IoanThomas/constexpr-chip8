add_executable(chip8-emu
    chip8.h
    config_file.cpp
    config_file.h
    emulator.cpp
    emulator.h
    main.cpp)
target_compile_features(chip8-emu PRIVATE cxx_std_17)
set_target_properties(chip8-emu PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(chip8-emu PRIVATE sfml-audio sfml-graphics)
target_include_directories(chip8-emu PRIVATE
    "${PROJECT_SOURCE_DIR}/extern/SFML/include")

add_custom_command(TARGET chip8-emu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/extern/SFML/extlibs/bin/x64/openal32.dll"
        $<TARGET_FILE_DIR:chip8-emu>)
        
add_custom_command(TARGET chip8-emu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/data/window.cfg"
        $<TARGET_FILE_DIR:chip8-emu>)
        
add_custom_command(TARGET chip8-emu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/data/keybinds.cfg"
        $<TARGET_FILE_DIR:chip8-emu>)
