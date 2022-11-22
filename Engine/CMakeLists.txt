project(Engine)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_compile_definitions(
    $<$<CONFIG:Debug>:NK_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:NK_RELEASE>
    $<$<CONFIG:Release>:NK_DIST>
)

if (WIN32)
    add_compile_definitions(
        NK_PLATFORM_WINDOWS
    )
elseif (UNIX AND NOT APPLE)
    add_compile_definitions(
        NK_PLATFORM_LINUX
    )
endif()

find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(GLSLC_EXC NAMES glslc HINTS Vulkan::glslc)

add_subdirectory(vendor/fmt)
add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/GLFW)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/tinyobjloader)
add_subdirectory(vendor/eventpp)

file(GLOB SOURCES CMAKE_CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/Core/*.cpp"
    "src/Core/Rendering/*.cpp"
    "src/Utilities/*.cpp"
)

add_library(Engine STATIC ${SOURCES})

target_link_libraries(Engine
PUBLIC
    fmt::fmt
    spdlog::spdlog_header_only
    glfw
    Vulkan::Vulkan
    glm
    tinyobjloader
)

target_precompile_headers(Engine PUBLIC src/nkpch.h)

target_include_directories(Engine
PUBLIC
    src

    # Libraries
    vendor/fmt/include
    vendor/spdlog/include
    vendor/eventpp/include
    vendor/glm
PRIVATE
    ${Vulkan_INCLUDE_DIR}
    vendor/GLFW/include
    vendor/stb
    vendor/tinyobjloader
)

target_include_directories(spdlog PRIVATE vendor/fmt/include)

if (UNIX AND NOT APPLE)
    set_property(TARGET fmt PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

#=======================================================================
# COMPILE SHADERS
#=======================================================================

if (WIN32)
    set(DXC_EXC "$ENV{VULKAN_SDK}/Bin/dxc.exe")
endif()

file(GLOB HLSL_SOURCE_FILES
    "shaders/*.vert.hlsl"
    "shaders/*.frag.hlsl"
)

file(GLOB GLSL_SOURCE_FILES
    "shaders/*.vert.glsl"
    "shaders/*.frag.glsl"
)

function(shader_profile PROFILE SHADER_STAGE)
    if(${SHADER_STAGE} STREQUAL "vert")
        set(${PROFILE} "vs_6_1" PARENT_SCOPE)
    elseif(${SHADER_STAGE} STREQUAL "frag")
        set(${PROFILE} "ps_6_1" PARENT_SCOPE)
    else()
        message("Error selecting shader profile: ${SHADER_STAGE}")
    endif()
endfunction(shader_profile)

foreach(HLSL ${HLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${HLSL} NAME_WLE)
    get_filename_component(SHADER_EXT ${FILE_NAME} LAST_EXT)
    string(REPLACE "." "" SHADER_STAGE ${SHADER_EXT})
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    shader_profile(PROFILE ${SHADER_STAGE})
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${DXC_EXC} -spirv -T ${PROFILE} -E main ${HLSL} -Fo ${SPIRV}
        DEPENDS ${HLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(HLSL)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME_WLE)
    get_filename_component(SHADER_EXT ${FILE_NAME} LAST_EXT)
    string(REPLACE "." "" SHADER_STAGE ${SHADER_EXT})
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${GLSLC_EXC} -fshader-stage=${SHADER_STAGE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(SHADERS DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Engine SHADERS)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
)