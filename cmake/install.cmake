if(UNIX)
    include(GNUInstallDirs)

    install(TARGETS alfred_app)
    install(FILES "${ALFRED_DESKTOP_FILE}" DESTINATION "share/applications")
    install(FILES "${ALFRED_ICON_FILE}" DESTINATION "share/icons/hicolor/scalable/apps")
elseif(WIN32)
    install(TARGETS alfred_app)
    install(FILES "${ALFRED_ICON_FILE}" DESTINATION "/")

    message(WARNING "Installation for Windows might not work properly")
endif()
