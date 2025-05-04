find_package(toml11 QUIET)
if (NOT toml11_FOUND)
    message(STATUS "Fetching toml11")
    FetchContent_Declare(
        toml11
        GIT_REPOSITORY https://github.com/ToruNiina/toml11
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(toml11)
else()
    message(STATUS "Found toml11")
endif()
