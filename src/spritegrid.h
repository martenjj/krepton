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

#ifndef SPRITEGRID_H
#define SPRITEGRID_H

#include <qframe.h>

#include "krepton.h"


class QMouseEvent;
class QPaintEvent;

class Sprites;


class SpriteGrid : public QFrame
{
	Q_OBJECT

signals:
	void pressedButton(int button,int x,int y);
	void changedCoordinates(int x,int y);

public:
	SpriteGrid(QWidget *parent = NULL);
	void setSprite(const Sprites *ss, Obj::Type tt) { sprites = ss; object = tt; update(); }

protected:
	void paintEvent(QPaintEvent *ev) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;
	void leaveEvent(QEvent *ev) override;

private:
	const Sprites *sprites;
	Obj::Type object;
	int border;
};

#endif							// !SPRITEGRID_H
