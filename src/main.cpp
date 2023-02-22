
//  qnetwalk/main.cpp
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#include <QApplication>
#if defined(ENABLE_NLS)
#	include <QLocale>
#	include <QTranslator>
#endif
#include <QTextStream>

#include "mainwindow.h"

#include <tuple>

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
    
#if defined(ENABLE_NLS)
    QString appdir   = app.applicationDirPath();

    QTranslator translator(&app);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    std::ignore = translator.load(QLocale(), "qnetwalk", "_", ":/i18n/");
#else
    if (!translator.load(QLocale(), "qnetwalk", "_", appdir + "/translations/")) {
        std::ignore = translator.load(QLocale(), "qnetwalk", "_", DATADIR "/translations/");
    }
#endif
    app.installTranslator(&translator);
#endif

    MainWindow window;
    window.show();

    return app.exec();
}

