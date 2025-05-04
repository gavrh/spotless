find_package(tomlplusplus QUIET)
if (NOT tomlplusplus_FOUND)
    message(STATUS "Fetching tomlplusplus")
    FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(tomlplusplus)
else()
    message(STATUS "Found tomlplusplus")
endif()
