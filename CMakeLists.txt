cmake_minimum_required(VERSION 3.10)

# meta
project(qnetwalk VERSION 1.6.0)

# depends
find_package(SDL2 REQUIRED)
include(FindPkgConfig)
pkg_check_modules(SDL2_MIXER SDL2_mixer REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Gui Widgets LinguistTools)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

# sources
set(QNETWALK_HEADERS
	src/cell.h
	src/mainwindow.h
	src/sound.h
)

set(QNETWALK_SOURCES
	src/cell.cpp
	src/main.cpp
	src/mainwindow.cpp
	src/sound.cpp
)

set(QNETWALK_RESOURCES
	qnetwalk.qrc
)

file(GLOB QNETWALK_TRANSLATIONS translations/*.ts)
set_source_files_properties(${QNETWALK_TRANSLATIONS} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/translations")

# targets
qt5_add_resources(QNETWALK_RESOURCES_RCC ${QNETWALK_RESOURCES})
qt5_create_translation(QNETWALK_TRANSLATIONS_QMS ${QNETWALK_SOURCES} ${QNETWALK_HEADERS} ${QNETWALK_TRANSLATIONS})

add_executable(qnetwalk ${QNETWALK_HEADERS} ${QNETWALK_SOURCES} ${QNETWALK_RESOURCES_RCC} ${QNETWALK_TRANSLATIONS_QMS})
target_link_libraries(qnetwalk Qt5::Gui Qt5::Widgets ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES})
target_include_directories(qnetwalk SYSTEM PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS})
target_compile_definitions(qnetwalk PRIVATE -DDATADIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/qnetwalk" -DVERSION="${CMAKE_PROJECT_VERSION}")
set_target_properties(qnetwalk PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQURED YES
	CXX_EXTENSIONS NO
)

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

# install
install(TARGETS qnetwalk RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES qnetwalk.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)

install(FILES pics/qnetwalk.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps)
install(FILES menu/qnetwalk.xpm DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps)
install(FILES menu/qnetwalk.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

install(DIRECTORY sounds DESTINATION ${CMAKE_INSTALL_DATADIR}/qnetwalk)
install(FILES ${QNETWALK_TRANSLATIONS_QMS} DESTINATION ${CMAKE_INSTALL_DATADIR}/qnetwalk/translations)
