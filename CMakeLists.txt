cmake_minimum_required(VERSION 3.10)
project(dummy_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_SOURCES "")
foreach(def ${SOURCE_FILES})
    list(APPEND ENGINE_SOURCES "${SOURCE_DIR}/${def}")
    message(STATUS "${SOURCE_DIR}/${def} file has been added")
endforeach()

# Create executable
add_executable(engine ${ENGINE_SOURCES})

find_package(sfml-all REQUIRED)

target_link_libraries(engine
    sfml-graphics
    sfml-window
    sfml-system
    stdc++fs
)

# Include directories
include_directories(include)

# Additional options
target_compile_features(engine PRIVATE cxx_std_17)