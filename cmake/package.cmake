if(UNIX)
    set(CPACK_SOURCE_GENERATOR "TGZ")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
endif()

#if(WIN32)
#    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Alfred")
#endif()

set(CPACK_SOURCE_IGNORE_FILES "build\/.*;\.env\/.*;\.git\/.*;analysis\/.*;\.vscode\/.*;\.idea\/.*;extern\/.*")
set(CPACK_PACKAGE_VENDOR "Simon")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/app/res/icon128.png")
set(CPACK_PACKAGE_DESCRIPTION "Alfred is a free and open source cross-platform synthesizer and sequencer program, which allows you to synthesize different instruments and to compose and play music.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A cross-platform synthesizer and sequencer program")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_EXECUTABLES "alfred;Alfred")
set(CPACK_VERBATIM_VARIABLES TRUE)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "simonmara.dev@gmail.com")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libfftw3-double3")

set(CPACK_RPM_PACKAGE_REQUIRES "fftw")
