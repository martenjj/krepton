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

#include <qscrollarea.h>

#include "krepton.h"

class MapEdit;
class Sprites;


class MapGrid : public QScrollArea
{
	Q_OBJECT

public:
	MapGrid(QWidget *parent = NULL);

	void setSprites(const Sprites *ss);
	void setMap(MapEdit *mm);
	void updatedCell(int x,int y);
	void showTransporters(bool state);
	void showSelectedTransporter(bool state);
	void showSpiritRoutes(bool state);
	void selectedTransporter(int i = -1);

protected:
	bool eventFilter(QObject *obj, QEvent *ev) override;
	void update();

	bool widgetPaintEvent(QPaintEvent *ev);
	bool widgetMouseButtonPressEvent(QMouseEvent *ev);
	bool widgetMouseMoveEvent(QMouseEvent *ev);
	bool widgetLeaveEvent(QEvent *ev);

signals:
	void pressedButton(int,int,int);
	void changedCoordinates(int,int);

private:
	MapEdit *map;
	const Sprites *sprites;
	bool showtrans;
	bool showsel;
	bool showspiritroutes;
	int xtrans,ytrans;
};

#endif							// !MAPGRID_H
