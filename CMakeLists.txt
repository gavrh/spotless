cmake_minimum_required(VERSION 3.11)
project(spotless CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
	set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0076 NEW)
endif()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET ON)

include(cmake/ftxui.cmake)
include(cmake/nlohmann.cmake)
include(cmake/curl.cmake)

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB HEADER_FILES src/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE CURL::libcurl
)
