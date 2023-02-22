
//  qnetwalk/mainwindow.h
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#	include <QHash>
#else
#	include <QMap>
#endif

class Cell;
class QAction;
class QLCDNumber;
class QSoundEffect;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
    virtual QMenu * createPopupMenu();
protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
private:
    enum Skill { Novice, Normal, Expert, Master };
    enum BoardSize
    {
        NoviceBoardSize = 5,
        NormalBoardSize = 7,
        ExpertBoardSize = 9,
        MasterBoardSize = 9
    };
    enum
    {
	NumHighscores   = 10,
	MinimumNumCells = 20
    };
    enum class SoundEffect {
        Click,
        Connect,
        Start,
        Turn,
        Win,
    };
    typedef QList<Cell *> CellList;
private slots:
    void  help();
    void  about();
    void  newGame();
    void  keyboard();
    void  noRotation();
    void  startRotation();
    void  finishRotation();
    void  openHomepage();
    void  showHighscores();
    void  triggeredSkill(QAction *);
private:
    Cell * uCell(Cell * cell, bool wrap) const;
    Cell * dCell(Cell * cell, bool wrap) const;
    Cell * lCell(Cell * cell, bool wrap) const;
    Cell * rCell(Cell * cell, bool wrap) const;
    bool   isGameOver();
    bool   updateConnections();
    void   setSkill(int s);
    void   addRandomDir(CellList & list);
    void   addHighscore(int score);
    void   dialog(const QString & caption, const QString & text);

    void initSoundEffect(SoundEffect effect, const QString& fileName);
    void playSoundEffect(SoundEffect effect);
private:
    int          skill;
    bool         wrapped;
    Cell *       root;
    Cell *       board[MasterBoardSize * MasterBoardSize];
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QHash<SoundEffect, QSoundEffect*> soundEffects;
#else
    // broken qHash of enum class
    QMap<SoundEffect, QSoundEffect*> soundEffects;
#endif
    QString      user;
#if defined(ENABLE_SOUND)
    QAction *    soundAction;
#endif
    QStringList  highscores;
    QLCDNumber * lcd;
};

#endif
