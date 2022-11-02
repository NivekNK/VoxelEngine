cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/Linux/${CMAKE_BUILD_TYPE})
endif()

if (WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/Windows/${CMAKE_BUILD_TYPE})
endif()

project(VoxelEngine)

add_subdirectory(Engine)
add_subdirectory(Game)
