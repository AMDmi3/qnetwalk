
//  qnetwalk/mainwindow.cpp
//  Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QEventLoop>
#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include <QTime>
#include <QToolBar>
#include <QUrl>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "mainwindow.h"
#include "sound.h"

static QMap<Cell::Dirs, Cell::Dirs> contrdirs;


MainWindow::MainWindow() : QMainWindow(0, Qt::Dialog)
{
    contrdirs[Cell::U] = Cell::D;
    contrdirs[Cell::R] = Cell::L;
    contrdirs[Cell::D] = Cell::U;
    contrdirs[Cell::L] = Cell::R;

    QString appDir = qApp->applicationDirPath();
    QString sndDir = appDir + "/sounds/";
    
    if(!QFile::exists(sndDir))
	sndDir = DATADIR "/sounds/";

    winSound     = new Sound((sndDir + "win.wav").toLatin1().data());
    turnSound    = new Sound((sndDir + "turn.wav").toLatin1().data());
    clickSound   = new Sound((sndDir + "click.wav").toLatin1().data());
    startSound   = new Sound((sndDir + "start.wav").toLatin1().data());
    connectSound = new Sound((sndDir + "connect.wav").toLatin1().data());

    QSettings settings("QNetWalk");
    user = settings.value("Username", getenv("USER")).toString();
    bool isSound = settings.value("Sound", true).toBool();

    highscores = settings.value("Highscores").toStringList();
    if(highscores.count() != NumHighscores * 8)
    {
        highscores.clear();
	for(int i = 1; i < 5; i++)
	{
            for(int scores = 20 * i; scores < 30 * i; scores += i)
	    {
		highscores.append("...");
		highscores.append(QString::number(scores));
	    }
	}
    }

    skill = settings.value("Skill", Novice).toInt();
    if((skill != Novice) && (skill != Normal) && (skill != Expert))
	skill = Master;

    for(const auto& argument: qApp->arguments().mid(1))
    {
	if(argument == "-novice")       
	    skill = Novice;
	else if(argument == "-amateur") 
	    skill = Normal;
	else if(argument == "-expert")  
	    skill = Expert;
	else if(argument == "-master")  
	    skill = Master;
	else if(argument == "-nosound") 
	    isSound = false;
	else 
        QTextStream(stderr) << "Unknown option: '" << argument << "'. Try -help.\n";
    }

    setWindowTitle(tr("QNetWalk"));
    setWindowIcon(QPixmap(":/pics/qnetwalk.png"));
    setFixedSize(minimumSizeHint());
    setFocusPolicy(Qt::StrongFocus);

    QMenu * gameMenu = new QMenu(tr("&Game"), this);
    QToolBar * toolbar = addToolBar("Toolbar");
    toolbar->setMovable(false);

    QAction * action;
    action = gameMenu->addAction(QPixmap(":/pics/newgame.png"), tr("&New"), this, SLOT(newGame()), QKeySequence("N"));
    toolbar->addAction(action);
    
    action = gameMenu->addAction(QPixmap(":/pics/highscores.png"), tr("&Highscores"), this, SLOT(showHighscores()), QKeySequence("H"));
    toolbar->addAction(action);

    soundAction = gameMenu->addAction(tr("&Sound"));
    soundAction->setShortcut(QKeySequence("S"));
    soundAction->setCheckable(true);
    soundAction->setChecked(isSound);

    gameMenu->addSeparator();
    gameMenu->addAction(QPixmap(":/pics/quit.png"), tr("&Quit"), qApp, SLOT(closeAllWindows()), QKeySequence("Q"));

    QActionGroup * actions = new QActionGroup(this);
    connect(actions, SIGNAL(triggered(QAction *)), SLOT(triggeredSkill(QAction *)));

    action = actions->addAction(tr("&Novice"));
    action->setShortcut(QKeySequence("1"));
    action->setData(Novice);
        
    action = actions->addAction(tr("&Amateur"));
    action->setShortcut(QKeySequence("2"));
    action->setData(Normal);
    
    action = actions->addAction(tr("&Expert"));
    action->setShortcut(QKeySequence("3"));
    action->setData(Expert);
    
    action = actions->addAction(tr("&Master"));
    action->setShortcut(QKeySequence("4"));
    action->setData(Master);
    
    foreach(QAction * a, actions->actions())
    {
	a->setCheckable(true);
	if(a->data() == skill)
	    a->setChecked(true);
    }

    QMenu * skillMenu = new QMenu(tr("&Skill"), this);
    skillMenu->addActions(actions->actions());

    QMenu * helpMenu = new QMenu(tr("&Help"), this);

    helpMenu->addAction(tr("&Rules of Play"), this, SLOT(help()), QKeySequence("F1"));
    helpMenu->addAction(QPixmap(":/pics/homepage.png"), tr("&Homepage"), this, SLOT(openHomepage()));
    helpMenu->addAction(tr("&Keyboard"), this, SLOT(keyboard()), QKeySequence("K"));
    helpMenu->addSeparator();
    helpMenu->addAction(QPixmap(":/pics/qnetwalk.png"), tr("&About QNetWalk"), this, SLOT(about()));
    helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));

    menuBar()->addMenu(gameMenu);
    menuBar()->addMenu(skillMenu);
    menuBar()->addMenu(helpMenu);

    lcd = new QLCDNumber(toolbar);
    lcd->setFrameStyle(QFrame::Plain);
    toolbar->addWidget(lcd);

    const int cellsize = QPixmap(":/pics/background.png").width();
    const int gridsize = cellsize * MasterBoardSize + 2;

    QGridLayout * grid = new QGridLayout;
    grid->setMargin(0);
    grid->setSpacing(0);

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    frame->setFixedSize(gridsize, gridsize);
    frame->setLayout(grid);
    setCentralWidget(frame);

    Cell::initPixmaps();
    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
    {
	board[i] = new Cell(frame, i);
	board[i]->setFixedSize(cellsize, cellsize);
        connect(board[i], SIGNAL(noRotation()), SLOT(noRotation()));
        connect(board[i], SIGNAL(startRotation()), SLOT(startRotation()));
        connect(board[i], SIGNAL(finishRotation()), SLOT(finishRotation()));
	grid->addWidget(board[i], i / MasterBoardSize, i % MasterBoardSize);
    }

    for(int y = 0; y < MasterBoardSize; y++)
    {
	for(int x = 0; x < MasterBoardSize; x++)
	{
            const int i = MasterBoardSize * y + x;
	    if(y == 0)
	    {
		if(x == 0)
		    board[i]->setShadow(Cell::TopLeftShadow);
                else if(x == MasterBoardSize - 1)
		    board[i]->setShadow(Cell::TopRightShadow);
		else
		    board[i]->setShadow(Cell::TopShadow);
	    }
	    else if(y == MasterBoardSize - 1)
	    {
		if(x == 0)
		    board[i]->setShadow(Cell::BottomLeftShadow);
                else if(x == MasterBoardSize - 1)
		    board[i]->setShadow(Cell::BottomRightShadow);
		else
		    board[i]->setShadow(Cell::BottomShadow);
	    }
            else if(x == 0)
		board[i]->setShadow(Cell::LeftShadow);
            else if(x == MasterBoardSize - 1)
		board[i]->setShadow(Cell::RightShadow);
            else
		board[i]->setShadow(Cell::NoShadow);
	}
    }
    srand(time(0));
    setSkill(skill);
}

MainWindow::~MainWindow()
{
    delete winSound;
    delete turnSound;
    delete clickSound;
    delete startSound;
    delete connectSound;
}


QMenu * MainWindow::createPopupMenu()
{
    return 0;
}


void MainWindow::triggeredSkill(QAction * action)
{
    setSkill(action->data().toInt());
}

void MainWindow::newGame()
{
    lcd->display(0);
    if(soundAction->isChecked()) 
	startSound->play();

    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
    {
	board[i]->setDirs(Cell::None);
	board[i]->setConnected(false);
        board[i]->setLocked(false);
	board[i]->setRoot(false);
    }

    const int size = (skill == Novice) ? NoviceBoardSize :
	(skill == Normal) ? NormalBoardSize :
	(skill == Expert) ? ExpertBoardSize : MasterBoardSize;

    const int start = (MasterBoardSize - size) / 2;
    const int rootrow = rand() % size;
    const int rootcol = rand() % size;

    root = board[(start + rootrow) * MasterBoardSize + start + rootcol];
    root->setConnected(true);
    root->setRoot(true);

    while(true)
    {
	for(int row = start; row < start + size; row++)
	    for(int col = start; col < start + size; col++)
		board[row * MasterBoardSize + col]->setDirs(Cell::Free);

	CellList list;
	list.append(root);
	if(rand() % 2) 
	    addRandomDir(list);

	while(!list.isEmpty())
	{
	    if(rand() % 2)
	    {
		addRandomDir(list);
		if(rand() % 2) 
		    addRandomDir(list);
		list.removeFirst();
	    }
	    else
	    {
		list.append(list.first());
		list.removeFirst();
	    }
	}

	int cells = 0;
	for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	{
	    Cell::Dirs d = board[i]->dirs();
	    if((d != Cell::Free) && (d != Cell::None)) 
		cells++;
	}
	if(cells >= MinimumNumCells) 
	    break;
    }

    Cell::setGameOver(false);
    Cell::setFocused(0);
    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	board[i]->rotate((rand() % 4) * 90);
    updateConnections();
}


bool MainWindow::updateConnections()
{
    bool newconnection[MasterBoardSize * MasterBoardSize];
    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	newconnection[i] = false;

    CellList list;
    if(!root->isRotated())
    {
        newconnection[root->index()] = true;
	list.append(root);
    }
    while(!list.isEmpty())
    {
	Cell* cell = list.first();
	Cell* ucell = uCell(cell, wrapped);
	Cell* rcell = rCell(cell, wrapped);
	Cell* dcell = dCell(cell, wrapped);
	Cell* lcell = lCell(cell, wrapped);

	if((cell->dirs() & Cell::U) && ucell && (ucell->dirs() & Cell::D) &&
	   !newconnection[ucell->index()] && !ucell->isRotated())
	{
	    newconnection[ucell->index()] = true;
            list.append(ucell);
	}
	if((cell->dirs() & Cell::R) && rcell && (rcell->dirs() & Cell::L) &&
	   !newconnection[rcell->index()] && !rcell->isRotated())
	{
	    newconnection[rcell->index()] = true;
            list.append(rcell);
	}
	if((cell->dirs() & Cell::D) && dcell && (dcell->dirs() & Cell::U) &&
	   !newconnection[dcell->index()] && !dcell->isRotated())
	{
	    newconnection[dcell->index()] = true;
            list.append(dcell);
	}
	if((cell->dirs() & Cell::L) && lcell && (lcell->dirs() & Cell::R) &&
	   !newconnection[lcell->index()] && !lcell->isRotated())
	{
	    newconnection[lcell->index()] = true;
            list.append(lcell);
	}
	list.removeFirst();
    }

    bool isnewconnection = false;
    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
    {
	if(!board[i]->isConnected() && newconnection[i])
	    isnewconnection = true;
	board[i]->setConnected(newconnection[i]);
    }
    return isnewconnection;
}


void MainWindow::addRandomDir(CellList& list)
{
    Cell * cell = list.first();
    Cell * ucell = uCell(cell, wrapped);
    Cell * rcell = rCell(cell, wrapped);
    Cell * dcell = dCell(cell, wrapped);
    Cell * lcell = lCell(cell, wrapped);

    typedef QMap<Cell::Dirs, Cell *> CellMap;
    CellMap freecells;

    if(ucell && ucell->dirs() == Cell::Free) 
	freecells[Cell::U] = ucell;
    if(rcell && rcell->dirs() == Cell::Free) 
	freecells[Cell::R] = rcell;
    if(dcell && dcell->dirs() == Cell::Free) 
	freecells[Cell::D] = dcell;
    if(lcell && lcell->dirs() == Cell::Free) 
	freecells[Cell::L] = lcell;
    if(freecells.isEmpty()) 
	return;

    CellMap::ConstIterator it = freecells.constBegin();
    for(int i = rand() % freecells.count(); i > 0; --i) 
	++it;

    cell->setDirs(Cell::Dirs(cell->dirs() | it.key()));
    it.value()->setDirs(contrdirs[it.key()]);
    list.append(it.value());
}


Cell * MainWindow::uCell(Cell * cell, bool wrap) const
{
    if(cell->index() >= MasterBoardSize)
	return board[cell->index() - MasterBoardSize];
    else if(wrap)
	return board[MasterBoardSize * (MasterBoardSize - 1) + cell->index()];
    else return 0;
}


Cell * MainWindow::dCell(Cell * cell, bool wrap) const
{
    if(cell->index() < MasterBoardSize * (MasterBoardSize - 1))
	return board[cell->index() + MasterBoardSize];
    else if(wrap)
	return board[cell->index() - MasterBoardSize * (MasterBoardSize - 1)];
    else return 0;
}


Cell * MainWindow::lCell(Cell * cell, bool wrap) const
{
    if(cell->index() % MasterBoardSize > 0)
	return board[cell->index() - 1];
    else if(wrap)
	return board[cell->index() - 1 + MasterBoardSize];
    else return 0;
}


Cell * MainWindow::rCell(Cell * cell, bool wrap) const
{
    if(cell->index() % MasterBoardSize < MasterBoardSize - 1)
	return board[cell->index() + 1];
    else if(wrap)
	return board[cell->index() + 1 - MasterBoardSize];
    else return 0;
}


void MainWindow::keyPressEvent(QKeyEvent * event)
{
    Cell * focused = Cell::focused();
    if(focused)
    {
	switch(event->key())
	{
	case Qt::Key_Up:
	    Cell::setFocused(uCell(focused, true));
	    break;
	case Qt::Key_Down:
	    Cell::setFocused(dCell(focused, true));
	    break;
	case Qt::Key_Left:
	    Cell::setFocused(lCell(focused, true));
	    break;
	case Qt::Key_Right:
	    Cell::setFocused(rCell(focused, true));
            break;
	case Qt::Key_PageUp:
	case Qt::Key_Home:
	case Qt::Key_Delete:
	case Qt::Key_Z:
	    focused->start(Cell::RotationToLeft);
            break;
	case Qt::Key_PageDown:
	case Qt::Key_End:
	case Qt::Key_X:
	    focused->start(Cell::RotationToRight);
            break;
	case Qt::Key_Return:
	case Qt::Key_Enter:
	case Qt::Key_Space:
	    focused->start(Cell::LockUnlock);
	}
    }
    else if(event->modifiers() == Qt::NoModifier)
	Cell::setFocused(board[MasterBoardSize * MasterBoardSize / 2]);
}


void MainWindow::noRotation()
{
    if(soundAction->isChecked())
	clickSound->play();
}


void MainWindow::startRotation()
{
    if(soundAction->isChecked())
	turnSound->play();
    updateConnections();
}


void MainWindow::finishRotation()
{
    if(updateConnections() && soundAction->isChecked())
	connectSound->play();
    lcd->display(lcd->intValue() + 1);
    if(isGameOver())
    {
	if(soundAction->isChecked())
	    winSound->play();
	QString score = highscores.at(2 * (skill + 1) * NumHighscores - 1);
	if(lcd->intValue() <= score.toInt())
	    addHighscore(lcd->intValue());
    }
}


bool MainWindow::isGameOver()
{
    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
    {
	const Cell::Dirs d = board[i]->dirs();
	if((d != Cell::Free) && (d != Cell::None) && !board[i]->isConnected())
            return false;
    }

    for(int i = 0; i < MasterBoardSize * MasterBoardSize; i++)
	board[i]->setLocked(true);
    Cell::setGameOver(true);
    Cell::setFocused(0);
    return true;
}


void MainWindow::showHighscores()
{
    addHighscore(0);
}


void MainWindow::addHighscore(int score)
{
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Highscores"));
    
    QGridLayout * grid = new QGridLayout(dialog);
    grid->setMargin(10);
    grid->setSpacing(5);
    grid->setColumnStretch(2, 1);
    grid->setRowStretch(NumHighscores + 3, 1);
    grid->setColumnMinimumWidth(0, 40);
    grid->setColumnMinimumWidth(2, 150);

    QLabel * label = new QLabel(dialog);
    label->setPixmap(QPixmap(":/pics/computer2.png"));
    grid->addWidget(label, 0, 0, 2, 1);
    label = new QLabel(QString("<h3>") + dialog->windowTitle(), dialog);
    grid->addWidget(label, 0, 1, 1, 3);

    const QString header = (skill == Novice) ? tr("Novices") :
	(skill == Normal) ? tr("Amateurs") :
	(skill == Expert) ? tr("Experts") : tr("Masters");

    grid->addWidget(new QLabel("<b>#", dialog), 1, 1);
    grid->addWidget(new QLabel("<b>" + header, dialog), 1, 2);
    grid->addWidget(new QLabel("<b>" + tr("Scores"), dialog), 1, 3);

    QFrame * frame = new QFrame(dialog);
    frame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    grid->addWidget(frame, 2, 1, 1, 3);

    QLineEdit * line = 0;
    int inserted = 0;
    int index = 2 * skill * NumHighscores;
    for(unsigned int i = 0; i < NumHighscores; i++)
    {
        label = new QLabel(QString::number(i + 1), dialog);
	grid->addWidget(label, i + 3, 1);

	int next = index;
	if((score > 0) && (score <= highscores.at(++next).toInt()) && !line)
	{
            inserted = index;
	    line = new QLineEdit(user, dialog);
	    grid->addWidget(line, i + 3, 2);
            label = new QLabel(QString::number(score), dialog);
	    grid->addWidget(label, i + 3, 3);
	}
	else
	{
	    grid->addWidget(new QLabel(highscores.at(index++), dialog), i + 3, 2);
	    grid->addWidget(new QLabel(highscores.at(index++), dialog), i + 3, 3);
	}
    }
    QPushButton * button = new QPushButton("OK", dialog);
    connect(button, SIGNAL(clicked()), dialog, SLOT(accept()));
    const int pos = NumHighscores + 4;
    grid->addWidget(button, pos, 0, 1, 4, Qt::AlignHCenter);
    dialog->exec();

    if(line)
    {
        user = line->text();
	highscores.insert(inserted, QString::number(score));
	highscores.insert(inserted, user);
	highscores.removeAt(index);
	highscores.removeAt(index);
    }
    delete dialog;
}


void MainWindow::setSkill(int s)
{
    skill = s;

    if(skill == Master) 
	wrapped = true;
    else 
	wrapped = false;
    newGame();
}


void MainWindow::closeEvent(QCloseEvent * event)
{
    QSettings settings("QNetWalk");
    settings.setValue("Skill", skill);
    settings.setValue("Username", user);
    settings.setValue("Highscores", highscores);
    settings.setValue("Sound", soundAction->isChecked());
    event->accept();
}


void MainWindow::openHomepage()
{
    static const QUrl url("https://github.com/AMDmi3/qnetwalk");

    if(!QDesktopServices::openUrl(url))
	QMessageBox::warning(this, tr("Error"),
			     tr("Could not launch your web browser.\n"
				"Please, check the BROWSER environment's variable."));
}


void MainWindow::help()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("Rules of Play"));
    box.setIconPixmap(QPixmap(":/pics/computer2.png"));
    box.setText("<h3>" + box.windowTitle() + "</h3><p>" +
		tr("You are the system administrator and your goal "
		   "is to connect each computer to the central server."
		   "<p>Click the right mouse's button for turning the cable"
		   " in a clockwise direction, and left mouse's button"
		   " for turning the cable in a counter-clockwise direction."
		   "<p>Start the LAN with as few turns as possible!"));
    box.exec();
}


void MainWindow::keyboard()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("Keyboard"));
    box.setIconPixmap(QPixmap(":/pics/computer2.png"));
    box.setText("<h3>" + box.windowTitle() +
		"</h3><p><b>Up, Down, Left, Right</b> - " +
		tr("Select a cell.") +
		"<p><b>PageUp, Home, Delete, Z</b> - " +
		tr("Rotate counter-clockwise.") +
		"<p><b>PageDown, End, X</b> - " +
		tr("Rotate clockwise.") +
		"<p><b>Enter, Space</b> - " +
		tr("Lock and unlock the cell."));
    box.exec();
}


void MainWindow::about()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About QNetWalk") + QString(" " VERSION));
    box.setIconPixmap(QPixmap(":/pics/computer2.png"));
    box.setText("<h3>" + box.windowTitle() + "</h3><p>" +
		tr("QNetWalk is a free Qt-version of the NetWalk game.") +
		"<p>Copyright (C) 2004-2013, Andi Peredri<p>" +
		"<p>Copyright (C) 2018, Dmitry Marakasov<p>" +
		tr("Homepage:") + " https://github.com/AMDmi3/qnetwalk<p>" +
                tr("This program is distributed under the terms of the "
		   "GNU General Public License."));
    box.exec();
}

