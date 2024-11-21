
# You are free to modify this file

cmake_minimum_required(VERSION 3.0)
project(game)
find_package(X11 REQUIRED)
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
file(GLOB SRC *.cpp lib/*.cpp)
add_compile_options(-std=c++20)
add_executable(game ${SRC})
target_link_libraries(game m X11)

include_directories(lib)

add_custom_target(run
    COMMAND game
    DEPENDS game
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running the game executable"
)