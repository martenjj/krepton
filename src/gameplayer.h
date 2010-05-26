////////////////////////////////////////////////////// -*- mode:c++; -*- ///
//  
//  KRepton - the classic Repton game for KDE
//  
//  Copyright (c) 1998 Sandro Sigala <sandro@sigala.it>
//  Copyright (c) 2003 Jonathan Marten <jjm@keelhaul.demon.co.uk>
//  
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  It is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING for further details.
//  If not, write to the Free Software Foundation, Inc., 59 Temple Place -
//  Suite 330, Boston, MA 02111-1307, USA.
//
////////////////////////////////////////////////////////////////////////////

#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#include <qwidget.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QTimerEvent>
#include <QKeyEvent>
#include <QPaintEvent>

#include "map1.h"

class Sprites;
class Episode;

class GamePlayer : public QWidget
{
	Q_OBJECT

public:
	enum State					// level play/finish state
	{
		Unplayed = 0,
		Started = 1,
		Playing = 2,
		Finished = 3
	};

	GamePlayer(QWidget *parent = NULL, const char *name = NULL);
	~GamePlayer();

	const QString loadEpisode(const Episode *e);
	const QString loadSprites(const Episode *e);
	const QString loadMaps(const Episode *e);
	const Episode *saveEpisode(const Episode *e);
	const Episode *saveEpisode(const QString name,const QString path);
	void setSprites(const Sprites *ss);
	void setMaps(const MapList ml);

	QStringList listLevels(const Episode *e);
	void recordLevel(GamePlayer::State state);

	void startGame(const Episode *e,int level = 0);
	void startGamePassword(const Episode *e,const QString& password);
	void pauseAction();
	void suicideAction();

	int getPoints() const { return (points); }
	int countLevels() const { return (maps.count()); }
	int lastLevel() const { return (currentlevel); }

	bool inGame() const { return (in_game); }
	Sprites *getSprites() const { return (sprites); }
	const MapList &getMaps() const { return (maps); }

signals:
	void changedPlayState(bool ingame,bool inpause);
	void changedGameState(bool playing);
	void changedState(bool,bool);
	void changedStats(int,int,int);
	void changedFlags(bool,bool);
	void changedLives(int);
	void gameOver();

public slots:
	void goUp();
        void goDown();
	void goLeft();
	void goRight();

protected:
	void timerEvent(QTimerEvent *e);
	void paintEvent(QPaintEvent *e);
	void keyPressEvent(QKeyEvent *e);

private:
	MapList maps;
	Sprites *sprites;

	MapPlay *currentmap;
	int currentlevel;
	int timerObjects,timerMonsters,timerEggs;
	int timerSecs,timerPlants;
	QString episodeName;
	bool in_game,in_pause;
	int lives;
	int seconds;
	int points;
	int idle;
	int diamonds;
	bool havekey,havecrown;

	void endedGame(const QString &how,bool finish = false);
};

#endif							// !GAMEPLAYER_H
