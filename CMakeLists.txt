cmake_minimum_required(VERSION 3.10)
project(marchphine)

#marchphine
file(GLOB MARCHPHINE_SOURCES
    "src/*.cpp"
    "src/*.c"
)
add_executable(marchphine ${MARCHPHINE_SOURCES})
target_include_directories(marchphine PRIVATE include)

find_package(glfw3 REQUIRED)
target_link_libraries(marchphine PRIVATE glfw)