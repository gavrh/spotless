cmake_minimum_required(VERSION 3.11)
project(spotless CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WARN_DEPRECATED OFF)
if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
	set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0076 NEW)
endif()
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET ON)

include(cmake/librespot.cmake)
include(cmake/ftxui.cmake)
include(cmake/nlohmann.cmake)
include(cmake/curl.cmake)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS src/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE librespot
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE CURL::libcurl
)
