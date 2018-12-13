# QNetWalk

<a href="https://repology.org/metapackage/qnetwalk"><img src="https://repology.org/badge/vertical-allrepos/qnetwalk.svg" alt="Packaging status" align="right"></a>

[![Build Status](https://travis-ci.org/AMDmi3/qnetwalk.svg?branch=master)](https://travis-ci.org/AMDmi3/qnetwalk)

QNetWalk is a game for system administrators.

Copyright (C) 2004-2013, Andi Peredri <andi@ukr.net>

Currently maintained by [Dmitry Marakasov](https://github.com/AMDmi3) <amdmi3@amdmi3.ru>

## Requirements
 * CMake
 * Qt 5
 * SDL-mixer 1.2

## Installation
```shell
cmake.
make
make install  (optional)
update-menus  (optional)
```

## Features
 * Built-in levels generator.
 * Novice, Amateur, Expert, and Master skill levels.
 * Auto saving of settings.
 * Turn animation.
 * Highscores.
 * Sound support.
 * Command line support.
 * International languages support.

## Translators
 * Br.Portuguese:	Paulo Vitor <paulovitorls@globo.com>
 * German:		Thomas Loeber <ifp@loeber1.de>
 * Dutch:		Tom Scheper <scheper@gmail.com>
 * English:	Andi Peredri <andi@ukr.net>
 * French:		Habana <abcdelasecurite@free.fr>
 * Italian:	Alessandro Briosi <tsdogs@briosix.org>
 * Polish:  	Aleksander Wojdyga <awojdyga@liza.umcs.lublin.pl>
 * Russian: Andi Peredri <andi@ukr.net>
 * S.Chinese:	Carl_xwz <carl_xwz@163.com>
 * Spanish: Ismael Asensio <iasefue@ribera.tel.uva.es>
 * Turkish: Yenay Orhan <orhany45@yahoo.com>
 * Ukrainian:	L.Marvell <l.marvell@gmail.com>

## Translation
XXX: this section needs updating

 * Add new entry in the TRANSLATIONS section of the qnetwalk.pro file
 * lupdate qnetwalk.pro
 * linguist qnetwalk_xx.ts
 * lrelease qnetwalk.pro

## Contributors
 * SDL-mixer support: Dmitriy Poltavchenko <zen@root.ua>

## Environment
 * **USER** - default user name
 * **BROWSER** - default browser

## Ports
 * KDE:		Thomas Nagy <tnagy2^8@yahoo.fr>
 * NetBSD:		Ian Zagorskih <ianzag@megasignal.com>
 * OS/2:		Salvador Parra Camacho <sparrac@gmail.com>
 * Symbian:	Ahmad Mushtaq <ahmad.mushtaq@gmail.com>
 * Android:	Ian Smith <captain@moonblink.info>

## Sounds
Current sounds have been taken from
[LBreakout2](http://lgames.sourceforge.net/LBreakout2/) game.

If you have some free and interesting sounds for this game, feel
free to send pull requests.

# Graphics
 * Andi Peredri <andi@ukr.net>

# License
QNetWalk is distributed under the GNU General Public License, version 2.
See [COPYING](COPYING) for details.

# Homepage
Official [homepage](http://qt.osdn.org.ua/qnetwalk.html) for QNetWalk is dead since 2013.
Last [snapshot](https://web.archive.org/web/20131025174600/http://qt.osdn.org.ua/qnetwalk.html) of it is available in archive.org.
