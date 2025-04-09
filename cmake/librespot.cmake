find_package(librespot QUIET)
if (NOT librespot_FOUND)
    FetchContent_Declare(librespot
        GIT_REPOSITORY https://github.com/gavrh/librespot-c
        GIT_TAG master
    )
    FetchContent_MakeAvailable(librespot)
endif()
