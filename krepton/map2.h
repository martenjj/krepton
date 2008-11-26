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

#ifndef MAPEDIT_H
#define MAPEDIT_H

#include <qptrlist.h>

#include "krepton.h"
#include "map.h"


class MapEdit : public Map
{
public:
	MapEdit(const Map &m);				// copy constructor
	MapEdit(int sx,int sy,const QString pw);	// create with size
	~MapEdit();

	int getWidth() const { return (width); }
	int getHeight() const { return (height); }

	bool setCell(int x,int y,Obj::Type obj)
	{
		if (x<0 || y<0 || x>=width || y>=height) return (false);
		if (ref(x,y)==obj) return (false);
		ref(x,y) = obj;
		return (true);
	}

	Obj::Type getCell(int x,int y)
	{
		return (xy(x,y));
	}

	QPtrList<Transporter> getTransportersList();
	void transporterInsert(int ox,int oy,int dx,int dy);
	void transporterGet(int item,int *ox,int *oy,int *dx = NULL,int *dy = NULL);
	void transporterChange(int item,int ox,int oy,int dx,int dy);
	void transporterRemove(int item);
	void changePassword(const QString &pw);
	void changeTime(int t);
};


class MapEditList : public QPtrList<MapEdit>
{
public:
	void mapInsert(int sx,int sy,const QString password);
	void mapRemove(int item);
	void mapMoveUp(int item);
	void mapMoveDown(int item);

	operator MapList();
};

typedef QPtrListIterator<MapEdit> MapEditListIterator;


#endif							// !MAPEDIT_H
