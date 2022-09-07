set(APPLET "taskbar")

set(HEADER
    Taskbar.h
    TaskButton.h
)

set(SOURCES
    Taskbar.cpp
    TaskButton.cpp
)

set(LIBRARIES
    ${LIBRARIES}
)

BUILD_EDE_APPLET(${APPLET})
