project(Game)

add_executable(Game src/Game.cpp)

target_compile_definitions(Game PRIVATE
    $<$<CONFIG:Debug>:NK_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:NK_RELEASE>
    $<$<CONFIG:Release>:NK_DIST>
)

if (WIN33)
    target_compile_definitions(Game PRIVATE
        NK_PLATFORM_WINDOWS
    )
endif()

if (UNIX AND NOT APPLE)
    target_compile_definitions(Game PRIVATE
        NK_PLATFORM_LINUX
    )
endif()

target_link_libraries(Game PRIVATE Engine)
