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

#ifndef MAP_H
#define MAP_H

#include <qlist.h>
#include <qbytearray.h>

#include "krepton.h"


struct Monster
{
	int xpos,ypos;					// current position
	Obj::Type type;
	Obj::Type sprite;
	Orientation::Type orientation;
        int hold;					// stand still after birth

	Monster(int x, int y, Obj::Type t)
		: xpos(x), ypos(y),
		  type(t), sprite(t),
		  orientation(Orientation::West),
                  hold(0)				{}
};

typedef QList<Monster *> MonsterList;


struct Transporter
{
	int orig_x,orig_y;
	int dest_x,dest_y;

	Transporter(int ox, int oy, int dx, int dy)
		: orig_x(ox), orig_y(oy),
		  dest_x(dx), dest_y(dy) {}
};

typedef QList<Transporter *> TransporterList;


class Map
{
	friend class CheckMap;

public:
	Map(const QString &path);			// create from file
	Map(const Map &m);				// copy constructor
	Map(int sx,int sy,const QByteArray &pw);	// create with size
	virtual ~Map();
	const QString loadStatus() const { return (status); }

	bool save(const QString &path) const;
	int getSeconds() const { return (num_secs); }
	const QString getPassword() const { return (password); }

protected:
	int num_secs;
	QByteArray password;
	int xstart,ystart;				// last appear/transport
	int width,height;				// map overall size
	TransporterList transporters;

	void findStart();
	void addTransporter(int ox,int oy,int dx,int dy);

	Obj::Type &ref(int x,int y) const
	{
		return (data[y*width+x]);
	}

	Obj::Type xy(int x,int y) const
	{
		if (x<0 || y<0 || x>=width || y>=height) return (Obj::Wall);
		return (ref(x,y));
	}

	bool blipTryDirection(const Monster *m, Orientation::Type dir = Orientation::None) const;
	bool blipGoDirection(Monster *m, Orientation::Type dir = Orientation::None);
	bool updateBlip(Monster *m);

	virtual bool blipHit(Monster *m, Obj::Type type)	{ Q_UNUSED(m); Q_UNUSED(type); return (false); }

public:
	static void deltaForDirection(Orientation::Type dir, int *xp, int *yp);
	static char showDirection(Orientation::Type dir);

	virtual bool isempty(int x,int y) const;
	virtual bool isempty(Obj::Type obj) const;

	void blipInitialDirection(Monster *m) const;

private:
	Obj::Type *data;
	QString status;
};

typedef QList<Map *> MapList;


#endif							// !MAP_H
