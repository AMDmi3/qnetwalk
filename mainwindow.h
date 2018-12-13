
//  qnetwalk/mainwindow.h
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

class Cell;
class QAction;
class QLCDNumber;
class Sound;


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
private:
    int          skill;
    bool         wrapped;
    Cell *       root;
    Cell *       board[MasterBoardSize * MasterBoardSize];
    Sound *      clickSound;
    Sound *      connectSound;
    Sound *      startSound;
    Sound *      turnSound;
    Sound *      winSound;
    QString      user;
    QAction *    soundAction;
    QStringList  highscores;
    QLCDNumber * lcd;
};

#endif
