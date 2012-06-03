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

#ifndef KREPTON_H
#define KREPTON_H

#include <kdebug.h>
#include <klocale.h>


class Obj
{
public:	enum Type					// available Repton objects
	{						// DO NOT CHANGE THE ORDER!
		Rock,					// allowed in map files...
		Diamond,
		Ground1,
		Ground2,
		Time,
		Skull,
		Empty,
		Wall,
		Wall_West,
		Wall_East,
		Wall_North,
		Wall_South,
		Wall_North_West,
		Wall_North_East,
		Wall_South_West,
		Wall_South_East,
		Filled_Wall,
		Filled_Wall_North_West,
		Filled_Wall_North_East,
		Filled_Wall_South_West,
		Filled_Wall_South_East,
		Wall_Mirror,
		Strongbox,
		Cage,
		Egg,
		Key,
		Plant,
		Bomb,
		Transport,
		Crown,
		Repton,
		Blip,

		Blip2,					// dynamic during game...
		Monster,
		Monster2,
		Repton_Look_Left,
		Repton_Look_Right,
		Repton_Go_Right1,
		Repton_Go_Right2,
		Repton_Go_Right3,
		Repton_Go_Right4,
		Repton_Go_Left1,
		Repton_Go_Left2,
		Repton_Go_Left3,
		Repton_Go_Left4,
		Repton_Go_Up1,
		Repton_Go_Up2,
		Broken_Egg,               

		num_sprites,
							// state tracking only...
		Broken_Egg1,				// broken egg at stage 1
		Broken_Egg2,				// broken egg at stage 2
		Broken_Egg3,				// broken egg at stage 3
		Falling_Egg,				// The egg is falling
		Repton_Idle1,				// looking L->R while idle
		Repton_Idle2				// looking R->L while idle
	};
};


class Orientation
{
public:	enum Type { None = -1, North, South, East, West };
};


class Cheat
{
public:
	enum Option
	{
		NoCheats = 0x00,
		//HarmlessPlant = 0x01,			// plants are harmless
		//HarmlessMonster = 0x02,			// monsters are harmless
		//HarmlessSpirit = 0x04,			// spirits are harmless
		//HarmlessSkull = 0x08,			// skulls are harmless
		CannotBeCrushed = 0x10,			// not crushed by falling object
		NoTimeLimit = 0x20,			// time limit ignored
		//NoReplicatingPlant = 0x40,		// plants do not replicate
	};

	Q_DECLARE_FLAGS(Options, Option)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Cheat::Options)


extern void reportError(const KLocalizedString &message,
                        const QString &filename = QString::null,
			bool system = true, bool fatal = false);

#endif							// !KREPTON_H
