if(UNIX)
    set(CMAKE_INSTALL_BINDIR "bin")

    install(TARGETS alfred)
    install(FILES "${ALFRED_DESKTOP_FILE}" DESTINATION "share/applications")
    install(FILES "${ALFRED_ICON_FILE}" DESTINATION "share/icons/hicolor/scalable/apps")
elseif(WIN32)
    message(WARNING "No installation for Windows yet")
endif()
