File(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(minesweeper STATIC)
add_library(game::minesweeper ALIAS minesweeper)

target_sources(minesweeper
    PRIVATE ${src}
)

target_include_directories(minesweeper
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)