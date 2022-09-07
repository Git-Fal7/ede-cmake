set(APPLET "startmenu")

set(HEADER
    ede-icon.h
    XdgMenuReader.h
    DesktopEntry.h
    MenuRules.h
)

set(SOURCES
    StartMenu.cpp
    XdgMenuReader.cpp
    DesktopEntry.cpp
    MenuRules.cpp
)

set(LIBRARIES
    ${LIBRARIES}
)

BUILD_EDE_APPLET(${APPLET})
