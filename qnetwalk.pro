
TEMPLATE	= app
CONFIG	       += qt warn_on
LIBS           += -lSDL_mixer -lSDL

HEADERS		= cell.h	\
		  mainwindow.h  \
		  sound.h

SOURCES		= cell.cpp	\
		  main.cpp	\
		  mainwindow.cpp \
		  sound.cpp

RESOURCES	= qnetwalk.qrc

TRANSLATIONS    = translations/qnetwalk_de.ts	\
		  translations/qnetwalk_es.ts	\
		  translations/qnetwalk_fr.ts	\
		  translations/qnetwalk_it.ts	\
		  translations/qnetwalk_nl.ts	\
		  translations/qnetwalk_pl.ts	\
		  translations/qnetwalk_pt_BR.ts \
		  translations/qnetwalk_ru.ts	\
		  translations/qnetwalk_tr.ts	\
		  translations/qnetwalk_ua.ts	\
		  translations/qnetwalk_zh_CN.ts

TARGET		= qnetwalk
target.path	= /usr/games
INSTALLS       += target

man.path	= /usr/share/man/man6
man.files	= qnetwalk.6
INSTALLS       += man

tr.path		= /usr/share/qnetwalk/translations
tr.files	= translations/*.qm
INSTALLS       += tr

sounds.path	= /usr/share/qnetwalk/sounds
sounds.files	= sounds/*.wav
INSTALLS       += sounds

link.path	= /usr/share/applications
link.files	= menu/qnetwalk.desktop
INSTALLS       += link

icon.path	= /usr/share/pixmaps
icon.files	= menu/qnetwalk.xpm
INSTALLS       += icon

menu.path	= /usr/share/menu
menu.files	= menu/qnetwalk
INSTALLS       += menu

