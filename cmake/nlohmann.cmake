find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
    message(STATUS "Fetching nlohmann_json...")
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()
