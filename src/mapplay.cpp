////////////////////////// -*- indent-tabs-mode:t; c-basic-offset:8; -*- ///
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

#include "mapplay.h"

#include <stdlib.h>

#include <qpainter.h>
#include <qpixmap.h>

#include "krepton.h"
#include "sounds.h"


//  These chances are a percentage per-thing, per-tick.
#define CHANCE_PLANT_REPRODUCE		10		// plant will reproduce (3s tick)
#define CHANCE_MONSTER_PURSUE		1		// monster avoids obstacle (0.3s tick)

//  These are the number of ticks (with their respective times above)
//  that the monster is not hostile after its birth (monster) or the
//  start of the level (plant).
#define BEGIN_MONSTER_GRACE		2		// monster still after birth
#define BEGIN_PLANT_GRACE		5		// no plant reproduce at start

#define chance(c)			((rand() % 100)<(c))


MapPlay::MapPlay(const Map &m) : Map(m)			// create from map
{
	qDebug() << "pw='" << m.getPassword() << "'";

	num_diamonds = 0;
	have_key = have_crown = false;
	currentRepton = Obj::Repton;
	levelfinished = false;
	plant_inhibit = BEGIN_PLANT_GRACE;
	cheats_used = Cheat::NoCheats;

	qDebug() << "done";
}



MapPlay::~MapPlay()
{
	qDebug() << "pw='" << getPassword() << "'";
	qDeleteAll(monsters);
	monsters.clear();
	qDebug() << "done";
}



void MapPlay::setCheats(Cheat::Options cheats)
{
	qDebug() << "cheats" << cheats;
	cheats_used = cheats;
}



void MapPlay::startGame()
{
	prepareMap();
	game_secs = game_points = 0;			// nothing accumulated yet

// This makes sure that KNotify is running at the start of the game.
// Otherwise, there may be a big delay when the first diamond is hit...
	Sound::self()->playSound(Sound::Start);
}


void MapPlay::restartGame()
{
	if (ref(xpos,ypos)==Obj::Repton) ref(xpos,ypos) = Obj::Empty;

	xpos = xstart; ypos = ystart;			// where last materialised
	ref(xpos,ypos) = Obj::Repton;
	currentRepton = Obj::Repton;

	how_died.clear();
	game_secs = game_points = 0;			// nothing accumulated yet
}


const Transporter *MapPlay::findTransporter(int x,int y)
{
        for (TransporterList::const_iterator it = transporters.constBegin();
		it!=transporters.constEnd(); ++it)
	{
		const Transporter *tr = (*it);
		if (tr->orig_x==x && tr->orig_y==y) return (tr);
	}

	qDebug() << "Inconsistent map, no transporter at " << x << "," << y;
	return (NULL);
}


void MapPlay::prepareMap()
{
	qDebug();

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

        for (MonsterList::iterator it = monsters.begin();
		it!=monsters.end(); ++it)
	{
		Monster *m = (*it);
                if (m==NULL) continue;
		if (m->type==Obj::Blip) blipInitialDirection(m);
	}

	qDebug() << "done monsters=" << monsters.count() << " diamonds=" << num_diamonds;
}


Monster *MapPlay::findMonster(int x,int y)
{
        for (MonsterList::iterator it = monsters.begin();
		it!=monsters.end(); ++it)
	{
		Monster *m = (*it);
                if (m==NULL) continue;
		if (m->xpos==x && m->ypos==y) return (m);
	}
	return (NULL);
}


void MapPlay::addMonster(int x, int y, Obj::Type type)
{
	qDebug() << "xy=" << x << "," << y << " type=" << type;

	Monster *m = new Monster(x,y,type);
        if (type==Obj::Monster) m->hold = BEGIN_MONSTER_GRACE;
	monsters.append(m);
}


bool MapPlay::tryBreakEgg(int x,int y)
{
	Obj::Type obj = xy(x,y);

	if (obj!=Obj::Falling_Egg) return (false);

	Sound::self()->playSound(Sound::Egg);
	ref(x,y) = Obj::Broken_Egg;
	return (true);
}


// Try falling down.
bool MapPlay::tryFallDown(int x,int y)
{
	Obj::Type obj = xy(x,y);
	const Obj::Type next1 = xy(x,y+1);
	const Obj::Type next2 = xy(x,y+2);

	if (next1!=Obj::Empty) return (false);
	ref(x,y) = Obj::Empty;

	Monster *m;
	if ((m = findMonster(x,y+1))!=NULL && m->type==Obj::Monster)
	{
		Sound::self()->playSound(Sound::Monster);
		killMonster(m);
	}

//	Rock will land and stop on a broken egg, see comment in
//	tryFallLeftOrRight() below.

//	I think this handles the case where a falling egg lands on the
//      'bottom' of the scenario.  Falling egg landing elsewhere is handled
//      in the 'default' case in tryFallLeftOrRight() below.
	if (next2==Obj::Wall && (obj==Obj::Egg || obj==Obj::Falling_Egg))
	{
		Sound::self()->playSound(Sound::Egg);
		obj = Obj::Broken_Egg;
	}

	if (!(cheats_used & Cheat::CannotBeCrushed))
	{
		if (next2==Obj::Repton) die("You got crushed!");
	}

	if (obj==Obj::Egg) obj = Obj::Falling_Egg;
	ref(x,y+1) = obj;
	return (true);
}


// Check if the specified object can fall to the left or to the right.
bool MapPlay::tryFallHorizontal(int x, int y, int xd)
{
	Obj::Type obj = xy(x,y);

	if (xy(x+xd,y)==Obj::Empty && xy(x+xd,y+1)==Obj::Empty)
	{
		ref(x,y) = Obj::Empty;
		if (obj==Obj::Egg) obj = Obj::Falling_Egg;
		ref(x+xd,y) = obj;
		return (true);
	}

	return (false);
}


bool MapPlay::tryFallLeftOrRight(int x,int y)
{
	bool done = false;

	switch (xy(x,y+1))
	{
		// Curved objects.  Rocks and eggs can fall onto these objects.
		// If can't fall to the left, fall to the right.
case Obj::Rock:
case Obj::Skull:
case Obj::Diamond:
case Obj::Egg:
case Obj::Key:
case Obj::Bomb:
		// Broken eggs are not curved, rocks will land on top of them
		// and stay until they hatch.
		// See http://www.stairwaytohell.com/sthforums/viewtopic.php?f=1&t=2108
		if (!tryFallHorizontal(x,y,-1))
		{
			if (!tryFallHorizontal(x,y,1))
			{
				done = tryBreakEgg(x,y);
			}
			else done = true;
		}
		else done = true;
		break;

case Obj::Wall_North_West:
case Obj::Filled_Wall_North_West:
		if (!tryFallHorizontal(x,y,-1))
		{
			done = tryBreakEgg(x,y);
		}
		else done = true;
		break;		

case Obj::Wall_North_East:
case Obj::Filled_Wall_North_East:
		if (!tryFallHorizontal(x,y,1))
		{
			done = tryBreakEgg(x,y);
		}
		else done = true;
		break;

default:	done = tryBreakEgg(x,y);
	}

	return (done);
}


// Check if the specified object can fall.
bool MapPlay::tryFall(int x,int y)
{
	return (tryFallDown(x,y) || tryFallLeftOrRight(x,y));
}


// Kill a blip, make it a diamond, and produce a sound.
void MapPlay::cageBlip(Monster *m)
{
	Sound::self()->playSound(Sound::Cage);
	ref(m->xpos, m->ypos) = Obj::Diamond;
	killMonster(m);
}


/* override */ bool MapPlay::blipHit(Monster *m, Obj::Type obj)
{
	if (obj==Obj::Cage)
	{
		cageBlip(m);
	}
	else if (obj==Obj::Repton)
	{
		if (cheats_used & Cheat::HarmlessSpirit) return (false);
		die("It got you!");
	}

	return (true);
}


#define MONSTER_BLOCKED		INT_MAX			// a very large distance


// Check if the monster can go into a direction, and calculate the
// distance between it and Repton.  Return the square of the distance,
// this is only used in updateMonster() below for calculating the
// shortest distance.
int MapPlay::monsterTryDirection(const Monster *m, int xd, int yd)
{
	Obj::Type obj = xy(m->xpos+xd, m->ypos+yd);	// contents of destination

	if (isempty(obj))				// can move to there
	{						// calculate distance
		const int xdist = m->xpos+xd-xpos;
		const int ydist = m->ypos+yd-ypos;
		return (xdist*xdist + ydist*ydist);
        }
        else if (obj==Obj::Repton && !(cheats_used & Cheat::HarmlessMonster))
        {						// will move onto Repton
		die("The monster got you!");
        }
        return (MONSTER_BLOCKED);			// monster cannot move
}


// Check the Repton position and move the monster according with it.
bool MapPlay::updateMonster(Monster *m)
{
        if (m->hold>0) --m->hold;			// just count down, don't move
        else						// monster ready to move
        {
		int xd = 0;				// move that monster will make
		int yd = 0;

		const int xdist = m->xpos-xpos;
		const int ydist = m->ypos-ypos;
		int len = xdist*xdist + ydist*ydist;	// current distance to Repton

		// Try the four possible monster move directions
		// to see which one will bring it closer to Repton.
		int i = monsterTryDirection(m, 0, -1);
		if (i<len)
		{
			xd = 0; yd = -1; len = i;
		}

		i = monsterTryDirection(m, 0, 1);
		if (i<len)
		{
			xd = 0; yd = 1; len = i;
		}

		i = monsterTryDirection(m, 1, 0);
		if (i<len)
		{
			xd = 1; yd = 0; len = i;
		}

		i = monsterTryDirection(m, -1, 0);
		if (i<len)
		{
			xd = -1; yd = 0;
		}

		if (xd==0 && yd==0)			// monster can't move
		{
			// Blocked monster in a direct line can move down (if horizontal from
			// Repton) or right (if vertical from Repton).
			// See http://www.stairwaytohell.com/sthforums/viewtopic.php?f=1&t=2108
			// The % chance is my invention.
			if (m->ypos==ypos && chance(CHANCE_MONSTER_PURSUE) && monsterTryDirection(m,0,1)!=MONSTER_BLOCKED)
			{
				xd = 0; yd = 1;
			}
			else if (m->xpos==xpos && chance(CHANCE_MONSTER_PURSUE) && monsterTryDirection(m,1,0)!=MONSTER_BLOCKED)
			{
				xd = 1; yd = 0;
			}
		}

		m->xpos += xd;
		m->ypos += yd;
        }

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
			switch (xy(x,y))
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
	x += dx; y += dy;

	if (xy(x, y)==Obj::Repton)
	{
		if (!(cheats_used & Cheat::HarmlessPlant)) die("The plant got you!");
		return (true);
	}

	// Replicating plant can kill a monster, but not a blip
	// (see http://www.stairwaytohell.com/sthforums/viewtopic.php?f=1&t=2108)
	Monster *m;
	if ((m = findMonster(x, y))!=NULL && m->type==Obj::Monster)
	{
		Sound::self()->playSound(Sound::Monster);
		killMonster(m);
	}

	// Also, IIRC, it can replicate into earth
	if (isempty(x,y))
	{
		ref(x,y) = Obj::Plant;
		addMonster(x, y, Obj::Plant);
		return (true);
	}

	return (false);
}


// Check the plants and reproduce.
bool MapPlay::updatePlant(Monster *m)
{
	if (chance(CHANCE_PLANT_REPRODUCE))
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
        if (plant_inhibit-->0) return (false);		// grace time at start
	if (cheats_used & Cheat::NoReplicatingPlant) return (false);

	for (MonsterList::const_iterator it = monsters.constBegin();
             it!=monsters.constEnd(); ++it)
	{
		Monster *m = (*it);
                if (m==NULL) continue;
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

        for (MonsterList::iterator it = monsters.begin();
		it!=monsters.end(); ++it)
	{
		Monster *m = (*it);
                if (m==NULL) continue;
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
	switch (currentRepton)
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
		currentRepton = Obj::Repton;
		break;

case Obj::Repton:
		currentRepton = ((rand() % 1)==0 ? Obj::Repton_Look_Left : Obj::Repton_Look_Right);
		break;

case Obj::Repton_Look_Left:
		currentRepton = Obj::Repton_Idle1;
		break;

case Obj::Repton_Look_Right:
		currentRepton = Obj::Repton_Idle2;
		break;

case Obj::Repton_Idle1:
		currentRepton = Obj::Repton_Look_Right;
		break;

case Obj::Repton_Idle2:
		currentRepton = Obj::Repton_Look_Left;
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
	switch (currentRepton)
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
		currentRepton = Obj::Repton_Go_Up1;
		break;

case Obj::Repton_Go_Up1:
		currentRepton = Obj::Repton_Go_Up2;
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
		switch (currentRepton)
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
			currentRepton = Obj::Repton_Go_Left1;
			break;

case Obj::Repton_Go_Left1:
			currentRepton = Obj::Repton_Go_Left2;
			break;

case Obj::Repton_Go_Left2:
			currentRepton = Obj::Repton_Go_Left3;
			break;

case Obj::Repton_Go_Left3:
			currentRepton = Obj::Repton_Go_Left4;
			break;

default:		;				// Avoid warning
		}
	}
	else						// Move Repton to the right.
	{
		switch (currentRepton)
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
			currentRepton = Obj::Repton_Go_Right1;
			break;

case Obj::Repton_Go_Right1:
			currentRepton = Obj::Repton_Go_Right2;
			break;

case Obj::Repton_Go_Right2:
			currentRepton = Obj::Repton_Go_Right3;
			break;

case Obj::Repton_Go_Right3:
			currentRepton = Obj::Repton_Go_Right4;
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

// This function is called when Repton gets an object.
void MapPlay::gotObject(Obj::Type obj)
{
	switch (obj)
	{
case Obj::Diamond:
		Sound::self()->playSound(Sound::Diamond);
		--num_diamonds;
		game_points += 50;
		break;

case Obj::Time:	Sound::self()->playSound(Sound::Time);
		game_secs += 30;
		game_points += 10;
		break;

case Obj::Key:	Sound::self()->playSound(Sound::Key);
		gotKey();
		game_points += 30;
		break;

case Obj::Crown:
		Sound::self()->playSound(Sound::Crown);
		game_points += 200;
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
        if (t==NULL)					// map inconsistent!
        {
		ref(x,y) = Obj::Repton;
		return;
        }

	// Remove the transporter and Repton from the original location.
	ref(x,y) = Obj::Empty;
	ref(xpos,ypos) = Obj::Empty;

	Sound::self()->playSound(Sound::Transport);

	xpos = t->dest_x; ypos = t->dest_y;		// move to destination
	xstart = xpos; ystart = ypos;			// record for reincarnation
	switch (xy(xpos,ypos))
	{
case Obj::Diamond:
case Obj::Time:
case Obj::Key:
case Obj::Crown:
		gotObject(xy(xpos,ypos));
		// Move Repton to the new location
		ref(xpos,ypos) = Obj::Repton;
		currentRepton = Obj::Repton;
		break;

case Obj::Ground1:
case Obj::Ground2:
		game_points += 2;
		/* FALLTHROUGH */

case Obj::Empty:
		// No simple way to fix this for HarmlessMonster cheat
		if (findMonster(xpos,ypos)!=NULL) die("You transported onto a monster!");
		// Move Repton to the new location
		ref(xpos,ypos) = Obj::Repton;
		currentRepton = Obj::Repton;
		break;

case Obj::Transport:
		useTransporter(xpos,ypos);
		break;

default:	die("You transported onto something!");
		break;
	}
}


// Check if Repton can move according with the xd/yd value,
// and take appropriate action.  Either 'xd' or 'yd' can be
// set, but the other must be zero (no diagonal moves).
bool MapPlay::movePlayer(int xd, int yd)
{
	Monster *m = findMonster(xpos+xd, ypos+yd);
	if (m!=NULL)
	{
		switch (m->type)
		{
case Obj::Plant:	if (cheats_used & Cheat::HarmlessPlant) return (false);
			break;

case Obj::Blip:		if (cheats_used & Cheat::HarmlessSpirit) return (false);
			break;

case Obj::Monster:	if (cheats_used & Cheat::HarmlessMonster) return (false);
			break;

default:		break;
		}

		die("You ran into a monster!");
		return (false);
	}

	Obj::Type obj = xy(xpos+xd, ypos+yd);
	switch (obj)
	{
case Obj::Diamond:
case Obj::Time:
case Obj::Crown:
case Obj::Key:	gotObject(obj);
		/* FALLTHROUGH */

case Obj::Empty:
		if (xd!=0) moveHorizontalDirect(xd);
		else moveVerticalDirect(yd);
		break;

case Obj::Ground1:
case Obj::Ground2:
		game_points += 2;
		if (xd!=0) moveHorizontalDirect(xd);
		else moveVerticalDirect(yd);
		break;

case Obj::Rock:
case Obj::Egg:
case Obj::Broken_Egg:
case Obj::Falling_Egg:
case Obj::Broken_Egg1:
case Obj::Broken_Egg2:
case Obj::Broken_Egg3:
		if (xd!=0) moveHorizontalMoveObj(xd, obj);
		break;

case Obj::Transport:
		useTransporter(xpos+xd, ypos+yd);
		break;

case Obj::Skull:
		if (!(cheats_used & Cheat::HarmlessSkull)) die("You ran into something nasty!");
		break;

case Obj::Plant:
		if (!(cheats_used & Cheat::HarmlessPlant)) die("You ran into a plant!");
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
	const int dest = xpos+xd*2;			// where pushed thing will end up
	if (xy(dest, ypos)==Obj::Empty)
	{
		Monster *m;
		if ((m = findMonster(dest, ypos))!=NULL && m->type==Obj::Monster)
		{
			Sound::self()->playSound(Sound::Monster);
			killMonster(m);
		}
		ref(dest, ypos) = obj;
		moveHorizontalDirect(xd);
	}
}


// Check if Repton can move vertically according with the yd value.
bool MapPlay::moveVertical(int yd)
{
	return (movePlayer(0, yd));
}


// Check if Repton can move horizontally according with the xd value.
bool MapPlay::moveHorizontal(int xd)
{
	return (movePlayer(xd, 0));
}


// Kill a monster.
void MapPlay::killMonster(Monster *mp)
{
	game_points += 500;
	// Do not remove the monster from the 'monsters' list,
	// because this function can be called while iterating over it.
	// Just deactivate it.
	monsters[monsters.indexOf(mp)] = NULL;
	delete mp;
}

void MapPlay::die(const QString &how)
{
	qDebug() << "how" << how;

	how_died = how;
	Sound::self()->playSound(Sound::Die);
}


void MapPlay::paintMap(QPainter *p, int w, int h, const Sprites *sprites, Sprites::GetFlag state)
{
	for (int y = 0; y<=(h/Sprites::sprite_height); ++y)
	{
		for (int x = 0; x<=(w/Sprites::sprite_width); ++x)
		{
			int mx = x + xpos - (w/Sprites::sprite_width/2);
			int my = y + ypos - (h/Sprites::sprite_height/2);

			Obj::Type obj = xy(mx,my);
			switch (obj)
			{
case Obj::Broken_Egg1:
case Obj::Broken_Egg2:
case Obj::Broken_Egg3:		obj = Obj::Broken_Egg;
				break;

case Obj::Falling_Egg:		obj = Obj::Egg;
				break;

case Obj::Repton:		obj = currentRepton;
				if (obj==Obj::Repton_Idle1 || obj==Obj::Repton_Idle2) obj = Obj::Repton;
				break;

default:			;			// Avoid warning
			}

			const Monster *m = findMonster(mx,my);
			if (m!=NULL && m->type==Obj::Monster) obj = m->sprite;

			const QPixmap &pix = sprites->get(obj, state);
			p->drawPixmap(x*Sprites::sprite_width,y*Sprites::sprite_height,pix);

			if (m!=NULL && m->type==Obj::Blip)
			{
				const QPixmap &pix2 = sprites->get(m->sprite, state);
				p->drawPixmap(x*Sprites::sprite_width, y*Sprites::sprite_height, pix2);
			}
		}
	}
}
