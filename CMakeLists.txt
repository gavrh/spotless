cmake_minimum_required(VERSION 3.10)
project(spotless CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB HEADER_FILES src/*.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
