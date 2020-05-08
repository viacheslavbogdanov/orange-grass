cmake_minimum_required(VERSION 3.16)

project(SkyCrasher)

set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/)

add_subdirectory(3rdparty)
add_subdirectory(sources/game)
add_subdirectory(sources/engine)

if(WIN32)
    add_subdirectory(sources/launcher/win32)
endif()

if(ANDROID)
    add_subdirectory(sources/launcher/android)
endif()

if(UNIX AND NOT ANDROID)
    add_subdirectory(sources/launcher/linux)
endif()
