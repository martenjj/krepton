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
	bool hasDied() const { return (how_died!=QString::null); }
	const QString &howDied() const { return (how_died); }

	int readSeconds() { int s = num_secs; num_secs = 0; return (s); }
	int readPoints() { int p = num_points; num_points = 0; return (p); }

	int getDiamonds() const { return (num_diamonds); }
	bool haveKey() const { return (have_key); }
	bool haveCrown() const { return (have_crown); }

	void paintMap(QPainter *p,int width,int height,const Sprites *sprites);

private:
	int num_points;
	int num_diamonds;
	bool have_key,have_crown;
	bool levelfinished;
	QString how_died;
        int plant_inhibit;

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
	bool blipTryDirection(const Monster *m, Orientation::Type dir = Orientation::None) const;
	bool blipGoDirection(Monster *m, Orientation::Type dir = Orientation::None);
	double monsterTryDirection(Monster *m, int xd, int yd);
	bool updateBlip(Monster *m);
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
	void killMonster(Monster *mp);
	void cageBlip(Monster *m, int x, int y);
	void die(const QString &how);

	bool isempty(Obj::Type obj) const
	{
		switch (obj)
		{
case Obj::Empty:
case Obj::Ground1:
case Obj::Ground2:	return (true);

default:		return (false);
		}
	}

	bool isempty(int x,int y) const
	{
		return (isempty(xy(x,y)));
	}
};


#endif							// !MAPPLAY_H
