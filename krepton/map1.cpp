////////////////////////////////////////////////////////////////////////////
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

#include "config.h"

#include <math.h>

#include <qpainter.h>
#include <qpixmap.h>

#include "krepton.h"
#include "sprites.h"
#include "sounds.h"

#include "map1.h"


MapPlay::MapPlay(const Map &m) : Map(m)			// create from map
{
	kdDebug(0) << k_funcinfo << "pw='" << m.getPassword() << "'" << endl;

	num_diamonds = 0;
	have_key = have_crown = false;
	sprite = Obj::Repton;
	how_died = QString::null;
	levelfinished = false;

//	monsters.setAutoDelete(true);
	kdDebug(0) << k_funcinfo << "done" << endl;
}



MapPlay::~MapPlay()
{
	kdDebug(0) << k_funcinfo << "pw='" << getPassword() << "'" << endl;
	monsters.setAutoDelete(true);
	monsters.clear();
	kdDebug(0) << k_funcinfo << "done" << endl;
}



void MapPlay::startGame()
{
	prepareMap();
	num_secs = num_points = 0;			// nothing accumulated yet

// This makes sure that KNotify is running at the start of the game.
// Otherwise, there may be a big delay when the first diamond is hit...
	Sound::playSound(Sound::Transport);
}


//void MapPlay::restartGame(const Map *m)
void MapPlay::restartGame()
{
	if (ref(xpos,ypos)==Obj::Repton) ref(xpos,ypos) = Obj::Empty;

	xpos = xstart; ypos = ystart;			// where last materialised
	ref(xpos,ypos) = Obj::Repton;
	sprite = Obj::Repton;

	how_died = QString::null;
//??????????	levelfinished = false;
	num_secs = num_points = 0;			// nothing accumulated yet
}


const Transporter *MapPlay::findTransporter(int x,int y)
{
	for (const Transporter *t = transporters.first(); t!=0; t = transporters.next())
	{
		if (t->orig_x==x && t->orig_y==y) return (t);
	}

	kdDebug(0) << k_funcinfo << "Inconsistent map, no transporter at " << x << "," << y << endl;
	return (NULL);
}

void MapPlay::prepareMap()
{
	kdDebug(0) << k_funcinfo << endl;

	findStart();
	xpos = xstart; ypos = ystart;			// set start position

	num_diamonds = 0;
	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			switch (ref(x,y))
			{
case Obj::Blip:			ref(x,y) = Obj::Empty;
				addMonster(x,y,Obj::Blip);
				break;

case Obj::Strongbox:
case Obj::Cage:
case Obj::Diamond:		++num_diamonds;
				break;

case Obj::Plant:		addMonster(x,y,Obj::Plant);
				break;

default:			;			// Avoid warning
			}
		}
	}

	for (Monster *m = monsters.first(); m!=NULL; m = monsters.next())
	{
		if (m->type!=Obj::Blip) continue;

		const int x = m->xpos;
		const int y = m->ypos;

		if (!isempty(x-1,y) && isempty(x,y-1)) m->orientation = Orientation::North;
		else if (!isempty(x,y-1) && isempty(x+1,y)) m->orientation = Orientation::East;	
		else if (!isempty(x+1,y) && isempty(x,y+1)) m->orientation = Orientation::South;
		else if (!isempty(x,y+1) && isempty(x-1,y)) m->orientation = Orientation::West;
	}

	kdDebug(0) << k_funcinfo << "done monsters=" << monsters.count() << " diamonds=" << num_diamonds << endl;
}


Monster *MapPlay::findMonster(int x,int y)
{
	for (Monster *m = monsters.first(); m!=NULL; m = monsters.next())
	{
		if (m->xpos==x && m->ypos==y) return (m);
	}
	return (NULL);
}

void MapPlay::addMonster(int x, int y, Obj::Type type)
{
	kdDebug(0) << k_funcinfo << "xy=" <<x << "," << y << " type=" << type << endl;
	monsters.append(new Monster(x,y,type));
}


// Try falling down.
bool MapPlay::tryFallDown(int x, int y)
{
	Obj::Type obj = xy(x,y);
	const Obj::Type next1 = xy(x,y+1);
	const Obj::Type next2 = xy(x,y+2);

	if (next1!=Obj::Empty) return (false);
	ref(x,y) = Obj::Empty;

	Monster *m;
	if ((m = findMonster(x,y+1))!=0 && m->type==Obj::Monster)
	{
		Sound::playSound(Sound::Kill_Monster);
		killMonster(m);
	}

	if (obj==Obj::Rock && (next2==Obj::Broken_Egg || next2==Obj::Broken_Egg1 ||
			       next2==Obj::Broken_Egg2 || next2==Obj::Broken_Egg3))
	{
		Sound::playSound(Sound::Kill_Monster);
		ref(x,y+2) = Obj::Empty;
	}

// TODO: falling egg can land on other things too?
	if (next2==Obj::Wall && (obj==Obj::Egg || obj==Obj::Falling_Egg))
	{
		Sound::playSound(Sound::Broken_Egg);
		obj = Obj::Broken_Egg;
	}

	if (next2==Obj::Repton) die("You got crushed!");

	if (obj==Obj::Egg) obj = Obj::Falling_Egg;
	ref(x,y+1) = obj;
	return (true);
}

// Check if the specified object can fall to the left or to the right.
bool MapPlay::tryFallHorizontal(int x, int y, int xd)
{
	const Obj::Type obj = xy(x,y);

	if (xy(x+xd,y+1)==Obj::Empty && xy(x+xd,y)==Obj::Empty)
	{
		ref(x,y) = Obj::Empty;
		if (obj==Obj::Egg) ref(x+xd,y) = Obj::Falling_Egg;
		else ref(x+xd,y) = obj;
		return (true);
	}

	return (false);
}

bool MapPlay::tryFallLeftOrRight(int x,int y)
{
	const Obj::Type obj = xy(x,y);
	bool done = false;

	switch(xy(x,y+1))
	{
case Obj::Rock:
case Obj::Skull:
case Obj::Diamond:
case Obj::Egg:
case Obj::Key:
case Obj::Bomb:
case Obj::Broken_Egg:
case Obj::Broken_Egg1:
case Obj::Broken_Egg2:
case Obj::Broken_Egg3:
		// Curved objects.  Rocks and eggs can fall onto these objects.
		// If can't fall to the left, fall to the right.
		if (!tryFallHorizontal(x,y,-1))
		{
			if (!tryFallHorizontal(x,y,1))
			{
				if (obj==Obj::Falling_Egg)
				{
					Sound::playSound(Sound::Broken_Egg);
					ref(x,y) = Obj::Broken_Egg;
					done = true;
				}
			}
			else done = true;
		}
		else done = true;
		break;

case Obj::Wall_North_West:
case Obj::Filled_Wall_North_West:
		if (!tryFallHorizontal(x,y,-1))
		{
			if (obj==Obj::Falling_Egg)
			{
				Sound::playSound(Sound::Broken_Egg);
				ref(x,y) = Obj::Broken_Egg;
				done = true;
			}
		}
		else done = true;
		break;		

case Obj::Wall_North_East:
case Obj::Filled_Wall_North_East:
		if (!tryFallHorizontal(x,y,1))
		{
			if (obj==Obj::Falling_Egg)
			{
				Sound::playSound(Sound::Broken_Egg);
				ref(x,y) = Obj::Broken_Egg;
				done = true;
			}
		}
		else done = true;
		break;

default:	if (obj==Obj::Falling_Egg)
		{
			Sound::playSound(Sound::Broken_Egg);
			ref(x,y) = Obj::Broken_Egg;
			done = true;
		}
	}

	return (done);
}

// Check if the specified object can fall.
bool MapPlay::tryFall(int x,int y)
{
//	if (!tryFallDown(x, y))
//		tryFallLeftOrRight(x, y);
	return (tryFallDown(x,y) || tryFallLeftOrRight(x,y));
}

// Check if the blip can go into a direction.
bool MapPlay::blipTryDirection(Monster *m,Orientation::Type dir,int xd,int yd)
{
	switch (xy(m->xpos+xd,m->ypos+yd))
	{
case Obj::Empty:
case Obj::Ground1:
case Obj::Ground2:
		m->xpos += xd;
		m->ypos += yd;
		m->orientation = dir;
		return (true);

case Obj::Cage:
		cageBlip(m, m->xpos + xd, m->ypos + yd);
		return (true);

case Obj::Repton:
		die("It got you!");
		break;

default:	;					// Avoid warning
	}

	return (false);
}

// Check and move the blip.
bool MapPlay::updateBlip(Monster *m)
{
	const int mx = m->xpos;
	const int my = m->ypos;

	switch (m->orientation)
	{
case Orientation::North:
		if (!blipTryDirection(m, Orientation::West,-1,0) &&
		    !blipTryDirection(m, Orientation::North,0,-1))
		{
			m->orientation = Orientation::East;
			return (updateBlip(m));
		}
		break;

case Orientation::South:
		if (!blipTryDirection(m, Orientation::East,1,0) &&
		    !blipTryDirection(m, Orientation::South,0,1))
		{
			m->orientation = Orientation::West;
			return (updateBlip(m));
		}
		break;

case Orientation::East:
		if (!blipTryDirection(m, Orientation::North,0,-1) &&
		    !blipTryDirection(m, Orientation::East,1,0))
		{
			m->orientation = Orientation::South;
			return (updateBlip(m));
		}
		break;

case Orientation::West:
		if (!blipTryDirection(m, Orientation::South,0,1) &&
		    !blipTryDirection(m, Orientation::West,-1,0))
		{
			m->orientation = Orientation::North;
			return (updateBlip(m));
		}
		break;
	}

	if (m->xpos != mx || m->ypos != my)
		m->sprite = (m->sprite==Obj::Blip) ? Obj::Blip2 : Obj::Blip;
	return (true);					// because of animation
}

// Check if the monster can go into a direction, and calculate the
// distance between it and Repton.
double MapPlay::monsterTryDirection(Monster *m, int xd, int yd)
{
	switch (xy(m->xpos+xd, m->ypos+yd))
	{
case Obj::Empty:
case Obj::Ground1:
case Obj::Ground2:
		return (sqrt(pow(m->xpos+xd-xpos,2) + pow(m->ypos+yd-ypos,2)));

case Obj::Repton:
		die("The monster got you!");

default:	;
	}

	return (666);					// A rather large number
}

// Check the Repton position and move the monster according with it.
bool MapPlay::updateMonster(Monster *m)
{
	int xd = 0;
	int yd = 0;
	double i,len;

	len = sqrt(pow(m->xpos-xpos,2) + pow(m->ypos-ypos,2));

	if ((i = monsterTryDirection(m, 0, -1))<len)
	{
		xd = 0; yd = -1; len = i;
	}
	if ((i = monsterTryDirection(m, 0, 1))<len)
	{
		xd = 0; yd = 1; len = i;
	}
	if ((i = monsterTryDirection(m, 1, 0))<len)
	{
		xd = 1; yd = 0; len = i;
	}
	if (monsterTryDirection(m, -1, 0)<len)
	{
		xd = -1; yd = 0;
	}

	m->xpos += xd;
	m->ypos += yd;
	m->sprite = (m->sprite==Obj::Monster) ? Obj::Monster2 : Obj::Monster;
	return (true);					// because of animation
}

// Check the broken eggs.
bool MapPlay::updateEggs()
{
	bool done = false;

	for (int y = height - 1; y >= 0; y--)
	{
		for (int x = width - 1; x >= 0; x--)
		{
			switch (xy(x,y))
			{
case Obj::Broken_Egg:		ref(x,y) = Obj::Broken_Egg1;
				done = true;
				break;

case Obj::Broken_Egg1:		ref(x,y) = Obj::Broken_Egg2;
				done = true;
				break;

case Obj::Broken_Egg2:		ref(x,y) = Obj::Broken_Egg3;
				done = true;
				break;

case Obj::Broken_Egg3:		ref(x,y) = Obj::Empty;
				addMonster(x,y,Obj::Monster);
				done = true;
				break;

default:			;			// Avoid warning
			}
		}
	}

	return (done);
}

// Check the moveable objects (rocks and eggs).
bool MapPlay::updateObjects()
{
	bool done = false;

	for (int y = height-2; y>=0; --y)		// Note the ``height-2'' value.
	{
		for (int x = width-1; x>=0; --x)
		{
			switch(xy(x,y))
			{
case Obj::Rock:						// Check if the object can fall
case Obj::Egg:
case Obj::Falling_Egg:
case Obj::Broken_Egg:
case Obj::Broken_Egg1:
case Obj::Broken_Egg2:
case Obj::Broken_Egg3:		if (tryFall(x,y)) done = true;
				break;

default:			;			// Avoid warning
			}
		}
	}

	return (done);
}

// Try to reproduce the plant.
bool MapPlay::tryPlant(int x, int y, int dx, int dy)
{
	if (xy(x+dx,y+dy)==Obj::Repton)
	{
		die("The plant got you!");
		return (true);
	}

	if (xy(x+dx,y+dy)==Obj::Empty)
	{
		ref(x+dx,y+dy) = Obj::Plant;
		addMonster(x+dx,y+dy,Obj::Plant);
		return (true);
	}

	return (false);
}

// Check the plants and reproduce.
bool MapPlay::updatePlant(Monster *m)
{
	if ((rand() % 100)<=10)
	{
		if (tryPlant(m->xpos, m->ypos, 1, 0)) return (true);
		else if (tryPlant(m->xpos, m->ypos, -1, 0)) return (true);
		else if (tryPlant(m->xpos, m->ypos, 0, 1)) return (true);
		else if (tryPlant(m->xpos, m->ypos, 0, -1)) return (true);
	}
	return (false);
}

// Reproduce the plants
bool MapPlay::updatePlants()
{
	for (Monster *m = monsters.first(); m!=0; m = monsters.next())
	{
		if (m->type==Obj::Plant)
		{
			if (updatePlant(m)) return (true);
		}
	}

	return (false);
}

// Check the monsters status and move them.
bool MapPlay::updateMonsters()
{
	bool done = false;

	for (Monster *m = monsters.first(); m!=0; m = monsters.next())
	{
		switch (m->type)
		{
case Obj::Blip:		if (updateBlip(m)) done = true;
			break;

case Obj::Monster:	if (updateMonster(m)) done = true;
			break;

default:		;				// Avoid warning
		}
	}

	return (done);
}


// Start looking look round if Repton has been idle for too long
bool MapPlay::updateIdle()
{
	switch (sprite)
	{
case Obj::Repton_Go_Right1:
case Obj::Repton_Go_Right2:
case Obj::Repton_Go_Right3:
case Obj::Repton_Go_Right4:
case Obj::Repton_Go_Left1:
case Obj::Repton_Go_Left2:
case Obj::Repton_Go_Left3:
case Obj::Repton_Go_Left4:
case Obj::Repton_Go_Up2:
case Obj::Repton_Go_Up1:
		sprite = Obj::Repton;
		break;

case Obj::Repton:
		sprite = ((rand() % 1)==0 ? Obj::Repton_Look_Left : Obj::Repton_Look_Right);
		break;

case Obj::Repton_Look_Left:
		sprite = Obj::Repton_Idle1;
		break;

case Obj::Repton_Look_Right:
		sprite = Obj::Repton_Idle2;
		break;

case Obj::Repton_Idle1:
		sprite = Obj::Repton_Look_Right;
		break;

case Obj::Repton_Idle2:
		sprite = Obj::Repton_Look_Left;
		break;

default:	;					// Avoid warning
	}

	return (true);
}


// Move Repton vertically.
void MapPlay::moveVerticalDirect(int yd)
{
	// Move Repton up or down according with the yd value.
	ref(xpos, ypos) = Obj::Empty;
	ref(xpos, ypos + yd) = Obj::Repton;
	ypos += yd;

	// Change the Repton sprite orientation.
	switch (sprite)
	{
case Obj::Repton:
case Obj::Repton_Look_Left:
case Obj::Repton_Look_Right:
case Obj::Repton_Go_Right1:
case Obj::Repton_Go_Right2:
case Obj::Repton_Go_Right3:
case Obj::Repton_Go_Right4:
case Obj::Repton_Go_Left1:
case Obj::Repton_Go_Left2:
case Obj::Repton_Go_Left3:
case Obj::Repton_Go_Left4:
case Obj::Repton_Go_Up2:
case Obj::Repton_Idle1:
case Obj::Repton_Idle2:
		sprite = Obj::Repton_Go_Up1;
		break;

case Obj::Repton_Go_Up1:
		sprite = Obj::Repton_Go_Up2;
		break;

default:	;					// Avoid warning
	}
}

// Move Repton horizontally.
void MapPlay::moveHorizontalDirect(int xd)
{
	// Move Repton to the left or to the right according
	// with the xd value.
	ref(xpos, ypos) = Obj::Empty;
	ref(xpos + xd, ypos) = Obj::Repton;
	xpos += xd;

	// Change the Repton sprite orientation.
	if (xd==-1)					// Move Repton to the left.
	{
		switch (sprite)
		{
case Obj::Repton:
case Obj::Repton_Look_Left:
case Obj::Repton_Look_Right:
case Obj::Repton_Go_Up1:
case Obj::Repton_Go_Up2:
case Obj::Repton_Go_Right1:
case Obj::Repton_Go_Right2:
case Obj::Repton_Go_Right3:
case Obj::Repton_Go_Right4:
case Obj::Repton_Go_Left4:
case Obj::Repton_Idle1:
case Obj::Repton_Idle2:
			sprite = Obj::Repton_Go_Left1;
			break;

case Obj::Repton_Go_Left1:
			sprite = Obj::Repton_Go_Left2;
			break;

case Obj::Repton_Go_Left2:
			sprite = Obj::Repton_Go_Left3;
			break;

case Obj::Repton_Go_Left3:
			sprite = Obj::Repton_Go_Left4;
			break;

default:		;				// Avoid warning
		}
	}
	else						// Move Repton to the right.
	{
		switch (sprite)
		{
case Obj::Repton:
case Obj::Repton_Look_Left:
case Obj::Repton_Look_Right:
case Obj::Repton_Go_Up1:
case Obj::Repton_Go_Up2:
case Obj::Repton_Go_Left1:
case Obj::Repton_Go_Left2:
case Obj::Repton_Go_Left3:
case Obj::Repton_Go_Left4:
case Obj::Repton_Go_Right4:
case Obj::Repton_Idle1:
case Obj::Repton_Idle2:
			sprite = Obj::Repton_Go_Right1;
			break;

case Obj::Repton_Go_Right1:
			sprite = Obj::Repton_Go_Right2;
			break;

case Obj::Repton_Go_Right2:
			sprite = Obj::Repton_Go_Right3;
			break;

case Obj::Repton_Go_Right3:
			sprite = Obj::Repton_Go_Right4;
			break;

default:		;				// Avoid warning
		}
	}
}

// This function is called when a key is taken. It makes the strongboxes into diamonds.
void MapPlay::gotKey()
{
	if (have_key) return;				// Already have the key

	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			if (xy(x,y)==Obj::Strongbox) ref(x,y) = Obj::Diamond;
		}
	}
	have_key = true;
}

// This function is called when Repton get an object.
void MapPlay::gotObject(Obj::Type obj)
{
	switch (obj)
	{
case Obj::Diamond:
		Sound::playSound(Sound::Got_Diamond);
		--num_diamonds;
		num_points += 50;
		break;

case Obj::Time:	Sound::playSound(Sound::Got_Time);
		num_secs += 30;
		num_points += 10;
		break;

case Obj::Key:	Sound::playSound(Sound::Got_Key);
		gotKey();
		num_points += 30;
		break;

case Obj::Crown:
		Sound::playSound(Sound::Got_Crown);
		num_points += 200;
		have_crown = true;
		break;

case Obj::Bomb:	if (num_diamonds==0) levelfinished = true;
		break;

default:	;					// Avoid warning
	}
}

// Move Repton to a new map location when it enters a transport.
void MapPlay::useTransporter(int x,int y)
{
	const Transporter *t = findTransporter(x,y);

	// Remove the transporter and Repton from the original location.
	ref(x,y) = Obj::Empty;
	ref(xpos,ypos) = Obj::Empty;

	Sound::playSound(Sound::Transport);

	xpos = t->dest_x; ypos = t->dest_y;		// move to destination
	xstart = xpos; ystart = ypos;			// record for reincarnation
	switch (xy(xpos,ypos))
	{
case Obj::Diamond:
case Obj::Time:
case Obj::Key:
case Obj::Crown:
		gotObject(xy(xpos,ypos));
		// Move Repton to the new location.
		ref(xpos,ypos) = Obj::Repton;
		sprite = Obj::Repton;
		break;

case Obj::Ground1:
case Obj::Ground2:
		num_points += 2;
		/* FALLTHROUGH */

case Obj::Empty:
		if (findMonster(xpos,ypos)!=NULL) die("You transported onto a monster!");
		// Move Repton to the new location.
		ref(xpos,ypos) = Obj::Repton;
		sprite = Obj::Repton;
		break;

case Obj::Transport:
		useTransporter(xpos,ypos);
		break;

default:	die("You transported onto something!");
		break;
	}
}

// Check if Repton can move vertically according with the yd value.
bool MapPlay::moveVertical(int yd)
{
	Obj::Type obj = xy(xpos,ypos+yd);

	if (findMonster(xpos,ypos+yd)!=NULL) die("You ran into a monster!");

	switch (obj)
	{
case Obj::Diamond:
case Obj::Time:
case Obj::Crown:
case Obj::Key:	gotObject(obj);
		/* FALLTHROUGH */

case Obj::Empty:
		moveVerticalDirect(yd);
		break;

case Obj::Ground1:
case Obj::Ground2:
		num_points += 2;
		moveVerticalDirect(yd);
		break;

case Obj::Transport:
		useTransporter(xpos, ypos + yd);
		break;

case Obj::Skull:
case Obj::Plant:
		die("You ran into something fatal!");
		break;

case Obj::Bomb:	gotObject(Obj::Bomb);
		break;

default:	return (false);				// unable to move
	}

	return (true);
}

// Move Repton and a near object horizontally.
void MapPlay::moveHorizontalMoveObj(int xd, Obj::Type obj)
{
	if (xy(xpos + xd*2, ypos)==Obj::Empty)
	{
		Monster *m;
		if ((m = findMonster(xpos + xd*2, ypos))!=0 && m->type==Obj::Monster)
		{
			Sound::playSound(Sound::Kill_Monster);
			killMonster(m);
		}
		ref(xpos + xd*2, ypos) = obj;
		moveHorizontalDirect(xd);
	}
}

// Check if Repton can move horizontally according with the xd value.
bool MapPlay::moveHorizontal(int xd)
{
	Obj::Type obj = xy(xpos+xd,ypos);

	if (findMonster(xpos + xd, ypos)!=NULL) die("You ran into a monster!");

	switch (obj)
	{
case Obj::Rock:
case Obj::Egg:
case Obj::Broken_Egg:
case Obj::Broken_Egg1:
case Obj::Broken_Egg2:
case Obj::Broken_Egg3:
		moveHorizontalMoveObj(xd, obj);
		break;

case Obj::Diamond:
case Obj::Time:
case Obj::Crown:
case Obj::Key:	gotObject(obj);
		/* FALLTHROUGH */

case Obj::Empty:
		moveHorizontalDirect(xd);
		break;

case Obj::Ground1:
case Obj::Ground2:
		num_points += 2;
		moveHorizontalDirect(xd);
		break;

case Obj::Transport:
		useTransporter(xpos + xd, ypos);
		break;

case Obj::Skull:
case Obj::Plant:
		die("You ran into something fatal!");
		break;

case Obj::Bomb:
		gotObject(Obj::Bomb);
		break;

default:	return (false);				// unable to move
	}

	return (true);
}

// Kill a monster.
void MapPlay::killMonster(Monster *mp)
{
	num_points += 500;
	monsters.remove(mp);
	delete mp;
}

// Kill a blip, make it a diamond, and produce a sound.
void MapPlay::cageBlip(Monster *m, int x, int y)
{
	Sound::playSound(Sound::Cage_Blip);
	ref(x,y) = Obj::Diamond;
	killMonster(m);
}

void MapPlay::die(const QString &how)
{
	kdDebug(0) << k_funcinfo << "how='" << how << "'" << endl;

	how_died = how;
	Sound::playSound(Sound::Die);
}



void MapPlay::paintMap(QPainter *p,int width,int height,const Sprites *sprites)
{
//	kdDebug(0) << k_funcinfo << endl;

	for (int y = 0; y<=(height/Sprites::sprite_height); ++y)
	{
		for (int x = 0; x<=(width/Sprites::sprite_width); ++x)
		{
			int mx = x + xpos - (width/Sprites::sprite_width/2);
			int my = y + ypos - (height/Sprites::sprite_height/2);

			Obj::Type obj = xy(mx,my);
			switch (obj)
			{
case Obj::Broken_Egg1:
case Obj::Broken_Egg2:
case Obj::Broken_Egg3:		obj = Obj::Broken_Egg;
				break;

case Obj::Falling_Egg:		obj = Obj::Egg;
				break;

case Obj::Repton:		obj = sprite;
				if (obj==Obj::Repton_Idle1 || obj==Obj::Repton_Idle2) obj = Obj::Repton;
				break;

default:			;			// Avoid warning
			}

			Monster *m;
			if ((m = findMonster(mx,my))!=NULL && m->type==Obj::Monster)
				obj = m->sprite;

			p->drawPixmap(x*Sprites::sprite_width,y*Sprites::sprite_height,sprites->get(obj));

			if (m!=NULL && m->type==Obj::Blip)
				p->drawPixmap(x*Sprites::sprite_width,y*Sprites::sprite_height,sprites->get(m->sprite));
		}
	}
}
