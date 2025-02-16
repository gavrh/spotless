find_program(CARGO_EXEC cargo)

if(NOT CARGO_EXEC)
    message(STATUS "Cargo not found, attempting to install it...")

    if(UNIX)
        message(STATUS "Instaling cargo via rustup...")
        execute_process(
            COMMAND curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE result
        )

        if(result)
            message(FATAL_ERROR "Failed to install cargo.")
        endif()
        set(ENV{PATH} "$ENV{HOME}/.cargo/bin:$ENV{PATH}")

    elseif(WIN32)
        message(STATUS "On Windows, please install cargo manually (via rustup) and ensure it's in the PATH.")
        message(FATAL_ERROR "Cargo not found and automatic installation is not supported on Windows in this script.")
    else()
        message(FATAL_ERROR "Unsupported platform. Please install cargo manually.")
    endif()
else()
    message(STATUS "Found cargo at ${CARGO_EXEC}")
endif()

