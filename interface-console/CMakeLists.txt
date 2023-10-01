File(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(interface-console)
add_executable(game::interface-console ALIAS interface-console)

target_sources(interface-console
    PRIVATE ${src}
)

target_include_directories(interface-console
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(interface-console
    PRIVATE game::minesweeper
)