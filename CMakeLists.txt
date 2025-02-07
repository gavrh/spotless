cmake_minimum_required(VERSION 3.5)
project(spotless CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB CXX_SOURCES src/*.cpp)
file(GLOB HXX_SOURCES src/*.h)

add_executable(${PROJECT_NAME} ${CXX_SOURCES} ${HXX_SOURCES})
