CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(EDE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
FIND_PACKAGE(PkgConfig REQUIRED)
FIND_PACKAGE(FLTK REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

pkg_check_modules(EDELIB REQUIRED edelib edelib-gui edelib-gui-no-images edelib-dbus)
pkg_check_modules(XKB REQUIRED xkbfile)

include("cmake/BuildApplet.cmake")

ADD_SUBDIRECTORY(ede-panel)
ADD_SUBDIRECTORY(ede-autostart)
ADD_SUBDIRECTORY(ede-dialog)
ADD_SUBDIRECTORY(ede-keyboard-conf)
ADD_SUBDIRECTORY(ede-timedate)
ADD_SUBDIRECTORY(ede-preferred-applications)
