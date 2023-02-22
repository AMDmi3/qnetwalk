
//  qnetwalk/main.cpp
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QTextStream>

#include "mainwindow.h"

int main(int argc, char ** argv)
{
    if((argc > 1) && QString(argv[1]) == "-help")
    {
        QTextStream(stderr) <<
         "Usage: qnetwalk [OPTIONS]\n"
		 "QNetWalk is a game for system administrators.\n"
		 "  -novice     set the skill Novice\n"
		 "  -amateur    set the skill Amateur\n"
		 "  -expert     set the skill Expert\n"
		 "  -master     set the skill Master\n"
		 "  -nosound    disable all sound effects\n"
		 "  -help       display this help and exit\n";
        return 1;
    }

    QApplication app(argc, argv);
    
    QString locale = QLocale::system().name();
    QString appdir   = app.applicationDirPath();
    QString filename = QString("qnetwalk_") + locale;
    
    QTranslator translator(&app);
#if (QT_VERSION>=QT_VERSION_CHECK(6,0,0))
    translator.load(filename, ":/i18n/");
#else
    if(!translator.load(filename, appdir + "/translations/"))
    translator.load(filename, DATADIR "/translations/");
#endif
    app.installTranslator(&translator);

    MainWindow window;
    window.show();

    return app.exec();
}

