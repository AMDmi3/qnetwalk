
//  qnetwalk/main.cpp
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char ** argv)
{
    if((argc > 1) && QString(argv[1]) == "-help")
    {
	qWarning("Usage: qnetwalk [OPTIONS]\n"
		 "QNetWalk is a game for system administrators.\n"
		 "  -novice     set the skill Novice\n"
		 "  -amateur    set the skill Amateur\n"
		 "  -expert     set the skill Expert\n"
		 "  -master     set the skill Master\n"
		 "  -nosound    disable all sound effects\n"
		 "  -help       display this help and exit\n");
        return 1;
    }

    QApplication app(argc, argv);
    
    QString locale = QLocale::system().name();
    QString qtdir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    
    QTranslator qtranslator(&app);
    qtranslator.load(QString("qt_") + locale, qtdir);
    app.installTranslator(&qtranslator);

    QString appdir   = app.applicationDirPath();
    QString filename = QString("qnetwalk_") + locale;
    
    QTranslator translator(&app);
    if(!translator.load(filename, appdir + "/translations/"))
	translator.load(filename, DATADIR "/translations/");
    app.installTranslator(&translator);

    MainWindow window;
    window.show();

    return app.exec();
}

