cmake_minimum_required(VERSION 3.10)

# meta
project(qnetwalk VERSION 1.9.0 LANGUAGES CXX)
include(GNUInstallDirs)

# options
option(USE_QT6 "Use Qt6 instead of Qt5" ON)
option(ENABLE_SOUND "Enable sound support (requires QtMultimedia)" ON)
option(ENABLE_NLS "I18n support" ON)

set(QNETWALK_QT_COMPONENTS Widgets)
set(QNETWALK_QT_LIBRARIES Qt::Widgets)
set(QNETWALK_COMPILE_DEFINITIONS
	DATADIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/qnetwalk"
	VERSION="${CMAKE_PROJECT_VERSION}"
)

if(ENABLE_SOUND)
	set(QNETWALK_QT_COMPONENTS ${QNETWALK_QT_COMPONENTS} Multimedia)
	set(QNETWALK_QT_LIBRARIES ${QNETWALK_QT_LIBRARIES} Qt::Multimedia)
	set(QNETWALK_COMPILE_DEFINITIONS ${QNETWALK_COMPILE_DEFINITIONS} ENABLE_SOUND)
endif()

if(ENABLE_NLS)
	set(QNETWALK_QT_COMPONENTS ${QNETWALK_QT_COMPONENTS} LinguistTools)
	set(QNETWALK_COMPILE_DEFINITIONS ${QNETWALK_COMPILE_DEFINITIONS} ENABLE_NLS)
endif()

# depends
if(USE_QT6)
	find_package(Qt6 REQUIRED COMPONENTS ${QNETWALK_QT_COMPONENTS})
	qt_standard_project_setup()
else()
	find_package(Qt5 REQUIRED COMPONENTS ${QNETWALK_QT_COMPONENTS})
	set(CMAKE_AUTOMOC ON)
endif()

# sources
set(QNETWALK_HEADERS
	src/cell.h
	src/mainwindow.h
)

set(QNETWALK_SOURCES
	src/cell.cpp
	src/main.cpp
	src/mainwindow.cpp
)

set(QNETWALK_RESOURCES
	qnetwalk.qrc
)

file(GLOB QNETWALK_TRANSLATIONS
	translations/*.ts
)

# targets
qt_add_resources(QNETWALK_RESOURCES_RCC ${QNETWALK_RESOURCES})

if(NOT USE_QT6 AND ENABLE_NLS)
	set_source_files_properties(${QNETWALK_TRANSLATIONS} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/translations")
	qt5_create_translation(QNETWALK_TRANSLATIONS_QMS ${QNETWALK_SOURCES} ${QNETWALK_HEADERS} ${QNETWALK_TRANSLATIONS})
endif()

add_executable(qnetwalk ${QNETWALK_HEADERS} ${QNETWALK_SOURCES} ${QNETWALK_RESOURCES_RCC} ${QNETWALK_TRANSLATIONS_QMS})
target_link_libraries(qnetwalk ${QNETWALK_QT_LIBRARIES})
target_compile_definitions(qnetwalk PRIVATE ${QNETWALK_COMPILE_DEFINITIONS})
set_target_properties(qnetwalk PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
)
if(USE_QT6 AND ENABLE_NLS)
	qt_add_translations(qnetwalk TS_FILES ${QNETWALK_TRANSLATIONS})
endif()

if(NOT MSVC)
	target_compile_options(qnetwalk PRIVATE -Wall -Wextra -pedantic)
endif()

if(NOT ${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	# make data available in the binary directory, so qnetwalk
	# may find it if run from there
	add_custom_target(link_sound ALL COMMAND
		${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/sounds ${CMAKE_BINARY_DIR}/sounds
	)
endif()

# installation
install(TARGETS qnetwalk RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES qnetwalk.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)

install(FILES pics/qnetwalk.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps)
install(FILES menu/qnetwalk.xpm DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps)
install(FILES menu/qnetwalk.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES menu/qnetwalk.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)

if(ENABLE_SOUND)
	install(DIRECTORY sounds DESTINATION ${CMAKE_INSTALL_DATADIR}/qnetwalk)
endif()
if(ENABLE_NLS)
	install(FILES ${QNETWALK_TRANSLATIONS_QMS} DESTINATION ${CMAKE_INSTALL_DATADIR}/qnetwalk/translations)
endif()
