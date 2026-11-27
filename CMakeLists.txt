cmake_minimum_required(VERSION 3.10)
project(marchphine)

file(GLOB_RECURSE MARCHPHINE_SOURCES
    "src/*.cpp"
    "src/*.c"
)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/dependencies/imgui")
file(GLOB IMGUI_SOURCES
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_executable(marchphine
    ${MARCHPHINE_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(marchphine PRIVATE
    include
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

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
