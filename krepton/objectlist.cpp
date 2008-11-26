////////////////////////////////////////////////////////////////////////////
//  
//  KReptonEd - editor for Repton levels for KDE
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

//#include <kapplication.h>
//#include <kglobal.h>
//#include <kstandarddirs.h>
//#include <kmessagebox.h>
//
//#include <qdir.h>
//#include <qfile.h>
//#include <qtstream.h>
#include <qstringlist.h>
//#include <qpainter.h>
//
//
#include "krepton.h"
#include "objectlist.h"


ObjectList *ObjectList::objectlist = NULL;


static const QString objnames =				// display names of objects
(							// map and sprite editors...
	"Rock,"						// Rock
	"Diamond,"					// Diamond
	"Earth 1,"					// Ground1
	"Earth 2,"					// Ground2
	"Time Bonus,"					// Time
	"Dangerous Skull,"				// Skull
	"Empty,"					// Empty
	"Wall,"						// Wall
	"Wall W,"					// Wall_West
	"Wall E,"					// Wall_East
	"Wall N,"					// Wall_North
	"Wall S,"					// Wall_South
	"Wall NW,"					// Wall_North_West
	"Wall NE,"					// Wall_North_East
	"Wall SW,"					// Wall_South_West
	"Wall SE,"					// Wall_South_East
	"Wall 2,"					// Filled_Wall
	"Wall 2 NW,"					// Filled_Wall_North_West
	"Wall 2 NE,"					// Filled_Wall_North_East
	"Wall 2 SW,"					// Filled_Wall_South_West
	"Wall 2 SE,"					// Filled_Wall_South_East
	"Wall 3,"					// Wall_Mirror
	"Strong Box,"					// Strongbox
	"Cage for Spirit,"				// Cage
	"Monster Egg,"					// Egg
	"Key for Strong Box,"				// Key
	"Replicating Plant,"				// Plant
	"Finish Object,"				// Bomb
	"Transporter,"					// Transport
	"Bonus Object,"					// Crown
	"Repton,"					// Repton
	"Spirit,"					// Blip
	","						// sprite editor only...
	"Spirit 2,"					// Blip2
	"Monster,"					// Monster
	"Monster 2,"					// Monster2
	"Repton Look Left,"				// Repton_Look_Left
	"Repton Look Right,"				// Repton_Look_Right
	"Repton Right 1,"				// Repton_Go_Right1
	"Repton Right 2,"				// Repton_Go_Right2
	"Repton Right 3,"				// Repton_Go_Right3
	"Repton Right 4,"				// Repton_Go_Right4
	"Repton Left 1,"				// Repton_Go_Left1
	"Repton Left 2,"				// Repton_Go_Left2
	"Repton Left 3,"				// Repton_Go_Left3
	"Repton Left 4,"				// Repton_Go_Left4
	"Repton Up/Down 1,"				// Repton_Go_Up1
	"Repton Up/Down 2,"				// Repton_Go_Up2
	"Broken Egg,"					// Broken_Egg
//							// future expansion...
//	"Broken Egg 1,"					// Broken_Egg1
//	"Broken Egg 2,"					// Broken_Egg3
//	"Broken Egg 3,"					// Broken_Egg3
//	"Falling Egg"					// Falling_Egg
);



ObjectList::ObjectList(const QString &s)
{
	kdDebug(0) << k_funcinfo << endl;

	lSprites = QStringList::split(',',s);
	lMaps = QStringList::split(',',s.section(",,",0,0));

	kdDebug(0) << k_funcinfo << "done n1=" << lSprites.count() << 
		" l2=" << lMaps.count() << endl;
}


const QStringList ObjectList::allSpriteNames()
{
	if (objectlist==NULL) objectlist = new ObjectList(objnames);
	return (objectlist->lSprites);
}

const QStringList ObjectList::allMapNames()
{
	if (objectlist==NULL) objectlist = new ObjectList(objnames);
	return (objectlist->lMaps);
}
