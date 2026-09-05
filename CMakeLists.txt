cmake_minimum_required(VERSION 3.10)
project(dummy_engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_SOURCES "")
foreach(def ${SOURCE_FILES})
    list(APPEND ENGINE_SOURCES "${SOURCE_DIR}/${def}")
    message(STATUS "${SOURCE_DIR}/${def} file has been added")
endforeach()

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(engine ${ENGINE_SOURCES})
target_compile_features(engine PRIVATE cxx_std_17)
target_link_libraries(engine PRIVATE SFML::Graphics SFML::Window SFML::System)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)