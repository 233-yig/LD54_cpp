include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    GDExtension
    GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
    GIT_TAG godot-4.1-stable
)
    
FetchContent_MakeAvailable(GDExtension)
    
File(GLOB_RECURSE src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    
add_library(interface-godot SHARED)
add_library(game::interface-godot ALIAS interface-godot)

target_sources(interface-godot
    PRIVATE ${src}
)

target_include_directories(interface-godot 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(interface-godot
    PRIVATE game::minesweeper
    PRIVATE godot::cpp
)