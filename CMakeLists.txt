cmake_minimum_required(VERSION 3.1)
project(raytracing)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/embree/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/libs/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/libs/gl3w)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stb)

file(GLOB srcfiles 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/libs/gl3w/GL/gl3w.c
)

add_executable(raytracing
    ${srcfiles}
)
target_link_libraries(raytracing ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/libs/glfw/lib-vc2010-64/glfw3.lib)
target_link_libraries(raytracing opengl32.lib)
target_link_libraries(raytracing ${CMAKE_CURRENT_SOURCE_DIR}/embree/bin/embree3.lib)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GL3W)

set_property(TARGET raytracing PROPERTY CXX_STANDARD 20)

#openmp
find_package(OpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")