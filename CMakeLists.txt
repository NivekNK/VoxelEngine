cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

function(get_os OS)
    if (UNIX AND NOT APPLE)
        set(${OS} "linux" PARENT_SCOPE)
    elseif (WIN32)
        set(${OS} "win32" PARENT_SCOPE)
    else()
        message("Error: Only Windows and linux are supported!")
    endif()
endfunction()

get_os(OS)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/${OS}-${CMAKE_BUILD_TYPE})

project(VoxelEngine)

add_subdirectory(Engine)
add_subdirectory(Game)
