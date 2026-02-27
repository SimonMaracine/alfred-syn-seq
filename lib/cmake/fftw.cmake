# Use the vendored libraries from the project only on Windows
# On Linux just use the system packages :P

if(WIN32)
    add_library(fftw3 INTERFACE)

    target_sources(
        fftw3
        INTERFACE
            FILE_SET HEADERS
            BASE_DIRS
                "extern/fftw"
            FILES
                "extern/fftw/fftw3.h"
    )

    target_link_directories(fftw3 INTERFACE "extern/fftw")

    set(ALFRED_FFTW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/extern/fftw/libfftw3-3.dll" PARENT_SCOPE)
endif()

macro(copy_fftw3_dll target)
    if(WIN32)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ALFRED_FFTW_DLL}"
                "$<TARGET_FILE_DIR:${target}>"
        )
    endif()
endmacro()
