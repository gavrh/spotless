find_package(CLI11 QUIET)
if (NOT CLI11_FOUND)
    message(STATUS "Fetching cli11")
    FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11
        GIT_TAG v2.5.0
    )
    FetchContent_MakeAvailable(CLI11)
else()
    message(STATUS "Found cli11")
endif()
