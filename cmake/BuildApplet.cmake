MACRO (BUILD_EDE_APPLET NAME)
    set(PROGRAM "ede-panel")
    project(${PROGRAM}_${NAME})

    set(PROG_SHARE_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/ede/panel-applets)

    list(FIND STATIC_PLUGINS ${NAME} IS_STATIC)
    set(SRC ${HEADERS} ${SOURCES} ${QM_LOADER} ${MOC_SOURCES} ${${PROJECT_NAME}_QM_FILES})
    if (${IS_STATIC} EQUAL -1) # not static
        add_library(${NAME} MODULE ${SRC}) # build dynamically loadable modules
        install(TARGETS ${NAME} DESTINATION ${PROG_SHARE_DIR}) # install the *.so file
    else() # static
        add_library(${NAME} STATIC ${SRC}) # build statically linked lib
    endif()
    target_link_libraries(${NAME} ${LIBRARIES})

ENDMACRO(BUILD_EDE_APPLET)
