cmake_minimum_required(VERSION 3.10)
project(marchphine)

file(GLOB MARCHPHINE_SOURCES
    "src/*.cpp"
    "src/*.c"
)
add_executable(marchphine ${MARCHPHINE_SOURCES})
target_include_directories(marchphine PRIVATE include)

if(WIN32)
    target_include_directories(marchphine PRIVATE "${CMAKE_SOURCE_DIR}/libs/glfw-win64/include")
    target_link_libraries(marchphine PRIVATE
        "${CMAKE_SOURCE_DIR}/libs/glfw-win64/libglfw3.a"
        opengl32
        gdi32
        user32
        shell32
        -static
    )
else()
    find_package(glfw3 REQUIRED)
    target_link_libraries(marchphine PRIVATE glfw)
endif()