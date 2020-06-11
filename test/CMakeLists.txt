add_executable(tests tests.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
set_target_properties(tests PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(tests PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/extern/SFML/include"
    "${PROJECT_SOURCE_DIR}/extern/catch2/single_include")
