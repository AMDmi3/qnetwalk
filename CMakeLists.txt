project(qnetwalk)
cmake_minimum_required(VERSION 3.10)

# depends
find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Gui Widgets LinguistTools)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

# sources
set(QNETWALK_HEADERS
	cell.h
	mainwindow.h
	sound.h
)

set(QNETWALK_SOURCES
	cell.cpp
	main.cpp
	mainwindow.cpp
	sound.cpp
)

set(QNETWALK_RESOURCES
	qnetwalk.qrc
)

set(QNETWALK_TRANSLATIONS
	translations/qnetwalk_de.ts
	translations/qnetwalk_es.ts
	translations/qnetwalk_fr.ts
	translations/qnetwalk_it.ts
	translations/qnetwalk_nl.ts
	translations/qnetwalk_pl.ts
	translations/qnetwalk_pt_BR.ts
	translations/qnetwalk_ru.ts
	translations/qnetwalk_tr.ts
	translations/qnetwalk_ua.ts
	translations/qnetwalk_zh_CN.ts
)
set_source_files_properties(${QNETWALK_TRANSLATIONS} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/translations")

# targets
qt5_add_resources(QNETWALK_RESOURCES_RCC ${QNETWALK_RESOURCES})
qt5_add_translation(QNETWALK_TRANSLATIONS_QMS ${QNETWALK_TRANSLATIONS})

add_executable(qnetwalk ${QNETWALK_HEADERS} ${QNETWALK_SOURCES} ${QNETWALK_RESOURCES_RCC} ${QNETWALK_TRANSLATIONS_QMS})
target_link_libraries(qnetwalk Qt5::Gui Qt5::Widgets ${SDL_LIBRARY} ${SDL_MIXER_LIBRARIES})
target_include_directories(qnetwalk SYSTEM PRIVATE ${SDL_INCLUDE_DIR} ${SDL_MIXER_INCLUDE_DIRS})
target_compile_definitions(qnetwalk PRIVATE -DDATADIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/qnetwalk")
set_target_properties(qnetwalk PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQURED YES
	CMAKE_CXX_EXTENSIONS NO
)

if(NOT ${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	add_custom_target(link_sound ALL COMMAND
		${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/sounds ${CMAKE_BINARY_DIR}/sounds
	)
endif()

# install
install(TARGETS qnetwalk RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES qnetwalk.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)

install(FILES menu/qnetwalk.xpm DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps)
install(FILES menu/qnetwalk.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

install(DIRECTORY sounds DESTINATION ${CMAKE_INSTALL_DATADIR}/qnetwalk)
install(FILES ${QNETWALK_TRANSLATIONS_QMS} DESTINATION ${CMAKE_INSTALL_DATADIR}/qnetwalk/translations)
