
//  qnetwalk/cell.h
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#ifndef CELL_H
#define CELL_H

#include <QPixmap>
#include <QWidget>


class Cell : public QWidget
{
    Q_OBJECT
public:
    enum Dirs { Free = 0, U = 1, R = 2, D = 4, L = 8, None = 16 };
    enum Shadow { NoShadow, TopShadow, BottomShadow, LeftShadow, RightShadow,
        TopLeftShadow, TopRightShadow, BottomLeftShadow, BottomRightShadow
    };
    enum {
	RotationTimeout = 18,
        RotationStep = 15,
	BlinkTimeout = 18,
	BlinkStep = 10,
        BlinkWidth = 6
    };
    enum Command {
	RotationToLeft,
	RotationToRight,
	LockUnlock
    };
    Cell(QWidget * parent, int i);
    int  index() const;
    void start(Command command);
    void rotate(int a);
    void setDirs(Dirs d);
    void setRoot(bool b);
    void setShadow(Shadow s);
    void setLocked(bool b);
    void setConnected(bool b);
    bool isConnected() const;
    bool isRotated() const;
    Dirs dirs() const;
    static void setFocused(Cell *);
    static void initPixmaps();
    static void setGameOver(bool b);
    static Cell * focused();
    static QPixmap fadedPixmap(const QPixmap &);
signals:
    void noRotation();
    void startRotation();
    void finishRotation();
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void timerEvent(QTimerEvent *);
private:
    typedef QMap<int, QPixmap *> PixmapMap;
    static  PixmapMap connectedpixmap;
    static  PixmapMap disconnectedpixmap;
    static  PixmapMap shadowpixmap;
    static  Cell * focusedCell;
    static  bool gameOver;
    int     iindex;
    int     rotationStep;
    int     rotationTimer;
    int     rotationProgress;
    int     focusTimer;
    int     focusProgress;
    int     blinkTimer;
    int     blinkProgress;
    bool    connected;
    bool    changed;
    bool    locked;
    bool    root;
    Dirs    ddirs;
    Shadow  shadow;
    QPixmap pixmap;
};

#endif

