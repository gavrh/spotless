message(STATUS "Fetching librespot...")
FetchContent_Declare(librespot
    GIT_REPOSITORY https://github.com/gavrh/librespot-cpp
    GIT_TAG master
)
FetchContent_MakeAvailable(librespot)
message(STATUS "librespot successfully fetched...")
