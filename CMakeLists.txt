cmake_minimum_required(VERSION 3.15)
project(3dgrapher)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lmingw32 -Wa,-mbig-obj")

add_executable(3dgrapher main.cpp
        imgui/imgui_impl_sdl.cpp
        imgui/imgui_impl_opengl3.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_widgets.cpp shaders.h shaderloader.cpp shaderloader.h camera.h shader.h
        tinyexpr.c
        tinyexpr.h)
target_link_libraries(3dgrapher OpenGL32 GLew32 sdl2main sdl2)
