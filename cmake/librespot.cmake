find_package(librespot QUIET)
if (NOT librespot_FOUND)
    message(STATUS "Fetching librespot")
    FetchContent_Declare(librespot
        GIT_REPOSITORY https://github.com/gavrh/librespot-c
        GIT_TAG master
    )
    FetchContent_MakeAvailable(librespot)
else()
    message(STATUS "Found librespot")
endif()
