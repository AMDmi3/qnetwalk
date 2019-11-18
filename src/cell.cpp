
//  qnetwalk/cell.cpp
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#include <QApplication>
#include <QImage>
#include <QMap>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#include "cell.h"


// Static ==============================================================

bool Cell::gameOver = true;

Cell::PixmapMap Cell::shadowpixmap;
Cell::PixmapMap Cell::connectedpixmap;
Cell::PixmapMap Cell::disconnectedpixmap;
Cell * Cell::focusedCell = 0;


void Cell::setGameOver(bool b)
{
    gameOver = b;
}


Cell * Cell::focused()
{
    return focusedCell;
}


void Cell::setFocused(Cell * cell)
{
    if(focusedCell)
    {
	Cell * previous = focusedCell;
	focusedCell = 0;
        previous->changed = true;
	previous->update();
    }
    if(cell && !gameOver)
    {
	focusedCell = cell;
        focusedCell->changed = true;
	focusedCell->update();
    }
}


void Cell::initPixmaps()
{
    typedef QMap<int, QString> NamesMap;
    NamesMap names;
    names[L]     = "0001";
    names[D]     = "0010";
    names[D|L]   = "0011";
    names[R]     = "0100";
    names[R|L]   = "0101";
    names[R|D]   = "0110";
    names[R|D|L] = "0111";
    names[U]     = "1000";
    names[U|L]   = "1001";
    names[U|D]   = "1010";
    names[U|D|L] = "1011";
    names[U|R]   = "1100";
    names[U|R|L] = "1101";
    names[U|R|D] = "1110";

    NamesMap::ConstIterator it;
    for(it = names.constBegin(); it != names.constEnd(); ++it)
    {
	connectedpixmap[it.key()] =
	    new QPixmap(QPixmap(":/pics/cable" + it.value() + ".png"));
	disconnectedpixmap[it.key()] =
	    new QPixmap(fadedPixmap(* connectedpixmap[it.key()]));
    }

    const int w = QPixmap(":/pics/background.png").width();
    QPixmap pixmap(":/pics/shadow.png");
    const int x1 = (pixmap.width() - w) / 2;
    const int x2 = pixmap.width() - w;
    shadowpixmap[NoShadow] = new QPixmap();
    shadowpixmap[TopLeftShadow]  = new QPixmap(pixmap.copy(0,  0,  w, w));
    shadowpixmap[TopShadow]      = new QPixmap(pixmap.copy(x1, 0,  w, w));
    shadowpixmap[TopRightShadow] = new QPixmap(pixmap.copy(x2, 0,  w, w));
    shadowpixmap[LeftShadow]     = new QPixmap(pixmap.copy(0,  x1, w, w));
    shadowpixmap[RightShadow]    = new QPixmap(pixmap.copy(x2, x1, w, w));
    shadowpixmap[BottomLeftShadow]  = new QPixmap(pixmap.copy(0,  x2, w, w));
    shadowpixmap[BottomShadow]      = new QPixmap(pixmap.copy(x1, x2, w, w));
    shadowpixmap[BottomRightShadow] = new QPixmap(pixmap.copy(x2, x2, w, w));

}


QPixmap Cell::fadedPixmap(const QPixmap & pixmap)
{
    QImage image = pixmap.toImage();
    for(int y = 0; y < image.height(); y++)
    {
	QRgb * line = (QRgb *)image.scanLine(y);
	for(int x = 0; x < image.width(); x++)
	{
	    QRgb pix = line[x];
	    if(qAlpha(pix) == 255)
	    {
		int g = (255 + 3 * qGreen(pix)) / 4;
		int b = (255 + 3 * qBlue(pix)) / 4;
		int r = (255 + 3 * qRed(pix)) / 4;
		line[x] = qRgb(r, g, b);
	    }
	}
    }
    return QPixmap::fromImage(image);
}


// Cell ================================================================


Cell::Cell(QWidget * parent, int i) : QWidget(parent)
{
    iindex    = i;
    ddirs     = Free;
    shadow    = NoShadow;
    changed   = true;
    connected = false;
    locked    = false;
    root      = false;

    blinkTimer       = 0;
    blinkProgress    = 0;
    focusTimer       = 0;
    focusProgress    = 0;
    rotationTimer    = 0;
    rotationProgress = 0;
}


void Cell::setShadow(Shadow s)
{
    shadow = s;
}


int Cell::index() const
{
    return iindex;
}


Cell::Dirs Cell::dirs() const
{
    return ddirs;
}


bool Cell::isConnected() const
{
    return connected;
}


bool Cell::isRotated() const
{
    return rotationProgress;
}


void Cell::setDirs(Dirs d)
{
    if(ddirs == d) return;
    ddirs = d;
    changed = true;
    update();
}


void Cell::setConnected(bool b)
{
    if(connected == b) return;
    connected = b;
    changed = true;
    update();
}


void Cell::setRoot(bool b)
{
    if(root == b) return;
    root = b;
    changed = true;
    update();
}


void Cell::setLocked(bool b)
{
    if(locked == b) return;
    locked = b;
    changed = true;
    update();
}


void Cell::paintEvent(QPaintEvent *)
{
    if(changed)
    {
	changed = false;

	if(ddirs == None || ddirs == Free || locked)
	    pixmap = QPixmap(":/pics/locked.png");
        else
	    pixmap = QPixmap(":/pics/background.png");

	QPainter paint;
	paint.begin(&pixmap);

	if(blinkProgress)
	{
	    paint.setPen(QPen(Qt::white, BlinkWidth));
	    paint.drawLine(0, width() - blinkProgress,
			   width(), 2 * width() - blinkProgress);
	}

	if(focusedCell == this)
	    paint.drawPixmap(0, 0, QPixmap(":/pics/focus.png"));

	if((ddirs != Free) && (ddirs != None))
	{
	    double offset = 0;
	    if(rotationProgress)
	    {
                offset = width() / 2;
		paint.translate(offset, offset);
		paint.rotate(rotationProgress);
	    }

	    if(connected)
		paint.drawPixmap(int(-offset), int(-offset), * connectedpixmap[ddirs]);
	    else 
		paint.drawPixmap(int(-offset), int(-offset), * disconnectedpixmap[ddirs]);
            paint.resetTransform();

	    if(root)
		paint.drawPixmap(0, 0, QPixmap(":/pics/server.png"));
	    else if(ddirs == U || ddirs == L || ddirs == D || ddirs == R)
	    {
		if(connected)
		    paint.drawPixmap(0, 0, QPixmap(":/pics/computer2.png"));
		else
		    paint.drawPixmap(0, 0, QPixmap(":/pics/computer1.png"));
	    }
	}
	paint.drawPixmap(0, 0, * shadowpixmap[shadow]);
	paint.end();
    }
    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
}


void Cell::mousePressEvent(QMouseEvent * e)
{
    setFocused(0);

    if(e->button() == Qt::LeftButton)
	start(RotationToLeft);
    else if(e->button() == Qt::RightButton)
	start(RotationToRight);
    else if(e->button() == Qt::MidButton)
	start(LockUnlock);
}


void Cell::start(Command command)
{
    if((ddirs == Free) || (ddirs == None) || gameOver)
    {
	if(blinkTimer)
            killTimer(blinkTimer);
        blinkTimer = startTimer(BlinkTimeout);
	emit noRotation();
    }
    else if(command == LockUnlock)
    {
	setLocked(!locked);
    }
    else if(locked)
    {
	if(blinkTimer)
            killTimer(blinkTimer);
        blinkTimer = startTimer(BlinkTimeout);
	emit noRotation();
    }
    else if(command == RotationToLeft)
    {
	if(rotationTimer)
            killTimer(rotationTimer);
	rotationStep = -RotationStep;
        rotationTimer = startTimer(RotationTimeout);
    }
    else if(command == RotationToRight)
    {
	if(rotationTimer)
            killTimer(rotationTimer);
	rotationStep = RotationStep;
        rotationTimer = startTimer(RotationTimeout);
    }
}


void Cell::timerEvent(QTimerEvent * e)
{
    if(e->timerId() == rotationTimer)
    {
	const bool start = rotationProgress ? false : true;
	rotate(rotationStep);

	if(start)
	    emit startRotation();

	if(((rotationStep > 0) && (rotationProgress > 0) && (rotationProgress < rotationStep)) ||
	   ((rotationStep < 0) && (rotationProgress < 0) && (rotationProgress > rotationStep)) ||
	   (rotationProgress == 0))
	{
	    killTimer(rotationTimer);
	    rotationTimer = 0;
	    rotationProgress = 0;
	    emit finishRotation();
	}
    }
    else if(e->timerId() == blinkTimer)
    {
	if(blinkProgress > 2 * width())
	{
	    killTimer(blinkTimer);
	    blinkTimer = 0;
            blinkProgress = 0;
	}
        else
	    blinkProgress += BlinkStep;
	changed = true;
	update();
    }
}


void Cell::rotate(int a)
{
    if((ddirs == Free) || (ddirs == None))
        return;

    rotationProgress += a;
    changed = true;
    while(rotationProgress >= 45)
    {
	rotationProgress -= 90;
	int newdirs = Free;
	if(ddirs & U) newdirs |= R;
	if(ddirs & R) newdirs |= D;
	if(ddirs & D) newdirs |= L;
	if(ddirs & L) newdirs |= U;
	setDirs(Dirs(newdirs));
    }
    while(rotationProgress < -45)
    {
	rotationProgress += 90;
	int newdirs = Free;
	if(ddirs & U) newdirs |= L;
	if(ddirs & R) newdirs |= U;
	if(ddirs & D) newdirs |= R;
	if(ddirs & L) newdirs |= D;
	setDirs(Dirs(newdirs));
    }
    update();
}

