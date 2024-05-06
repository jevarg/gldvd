cmake_minimum_required(VERSION 3.26)
project(gldvd)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        texture.h
        shaders.h
)

target_link_libraries(${PROJECT_NAME} OpenGL::OpenGL GLEW::GLEW GLUT::GLUT)
