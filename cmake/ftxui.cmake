find_package(ftxui QUIET)
if (NOT ftxui_FOUND)
    message(STATUS "Fetching ftxui...")
    FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v5.0.0
    )
    FetchContent_MakeAvailable(ftxui)
endif()
