message(STATUS "Installing librespot via cargo...")
execute_process(
    COMMAND ${CARGO_EXEC} install librespot --features pulseaudio-backend
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
if(result)
    message(FATAL_ERROR "Failed to install librespot using cargo, try installing manually.")
else()
    message(STATUS "librespot installed successfully.")
endif()
