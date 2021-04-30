///////////////// -*- mode:c++; indent-tabs-mode:t; c-basic-offset:8 -*- ///
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

#ifndef MAPPLAY_H
#define MAPPLAY_H

#include <qlist.h>

#include "krepton.h"
#include "map.h"


class QPainter;
class Sprites;


class MapPlay : public Map
{
public:
	MapPlay(const Map &m);
	~MapPlay();

	void startGame();
	void restartGame();

	bool updateEggs();
	bool updateObjects();
	bool updateMonsters();
	bool updatePlants();
	bool updateIdle();
	bool goUp() { return (moveVertical(-1)); }
        bool goDown() { return (moveVertical(1)); }
	bool goLeft() { return (moveHorizontal(-1)); }
	bool goRight() { return (moveHorizontal(1)); }

	bool hasEndedLevel() const { return (levelfinished); }
	bool hasDied() const { return (!how_died.isEmpty()); }
	const QString &howDied() const { return (how_died); }

	int gameSeconds() { int s = game_secs; game_secs = 0; return (s); }
	int gamePoints() { int p = game_points; game_points = 0; return (p); }

	int getDiamonds() const { return (num_diamonds); }
	bool haveKey() const { return (have_key); }
	bool haveCrown() const { return (have_crown); }

	void paintMap(QPainter *p,int width,int height,const Sprites *sprites, bool is_paused = false);

	void setCheats(Cheat::Options cheats);

private:
	int game_points;
	int game_secs;
	int num_diamonds;
	bool have_key,have_crown;
	bool levelfinished;
	QString how_died;
	int plant_inhibit;
	Cheat::Options cheats_used;
	int xpos,ypos;					// current Repton position
	Obj::Type currentRepton;			// sprite shown for him

	MonsterList monsters;

	Monster *findMonster(int x,int y);
	const Transporter *findTransporter(int x,int y);
	void addMonster(int x,int y,Obj::Type type);
	void prepareMap();

	bool tryBreakEgg(int x,int y);
	bool tryFallDown(int x,int y);
	bool tryFallHorizontal(int x,int y,int xd);
	bool tryFallLeftOrRight(int x,int y);
	bool tryFall(int x,int y);
	int monsterTryDirection(const Monster *m, int xd, int yd);
	bool updateMonster(Monster *m);
	bool tryPlant(int x, int y, int dx, int dy);
	bool updatePlant(Monster *m);
	void moveVerticalDirect(int yd);
	void moveHorizontalDirect(int xd);
	void gotKey();
	void gotObject(Obj::Type obj);
	void useTransporter(int x, int y);
	bool moveVertical(int yd);
	void moveHorizontalMoveObj(int xd, Obj::Type obj);
	bool moveHorizontal(int xd);
	bool movePlayer(int xd, int yd);
	void killMonster(Monster *mp);
	void cageBlip(Monster *m);
	void die(const QString &how);
	bool blipHit(Monster *m, Obj::Type obj) override;
};


#endif							// !MAPPLAY_H
