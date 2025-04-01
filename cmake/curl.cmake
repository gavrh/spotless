find_package(CURL QUIET)
if (NOT CURL_FOUND)
    FetchContent_Declare(
        CURL
        URL https://curl.se/download/curl-8.12.0.tar.xz
    )
    FetchContent_MakeAvailable(CURL)
endif()
