message(STATUS "Fetching libcurl...")
FetchContent_Declare(curl
    URL https://curl.se/download/curl-8.12.0.tar.xz
)
FetchContent_MakeAvailable(curl)
message(STATUS "libcurl successfully fetched...")
