////////////////////////////////////////////////////// -*- mode:c++; -*- ///
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

#ifndef MAPGRID_H
#define MAPGRID_H

#include <qscrollview.h>

#include "krepton.h"

class MapEdit;
class Sprites;

class MapGrid : public QScrollView
{
	Q_OBJECT

public:
	MapGrid(QWidget *parent = NULL,const char *name = NULL);
	void setSprite(Sprites *ss,Obj::Type tt) { sprites = ss; object = tt; }
	void setMap(MapEdit *mm);
	void updatedCell(int x,int y);
	void showTransporters(bool state);
	void showSelectedTransporter(bool state);
	void selectedTransporter(int i = -1);

signals:
	void pressedButton(int,int,int);
	void changedCoordinates(int,int);

protected:
	void drawContents(QPainter *p,int,int,int,int);
	void contentsMousePressEvent(QMouseEvent *e);
	void contentsMouseMoveEvent(QMouseEvent *e);

private:
	MapEdit *map;
	Sprites *sprites;
	Obj::Type object;
	bool showtrans;
	bool showsel;
	int xtrans,ytrans;
};

#endif							// !MAPGRID_H
