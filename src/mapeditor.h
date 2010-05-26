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

#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <qwidget.h>
//Added by qt3to4:
#include <QCloseEvent>

#include "krepton.h"

class SpritePreview;
class MapGrid;
class CoordLabel;
class ObjectListBox;
class Q3ButtonGroup;
class MapEdit;
class Sprites;

class MapEditor : public QWidget
{
	Q_OBJECT

public:
	MapEditor(QWidget *parent,Sprites **ss);
	void setMap(MapEdit *mm);

public slots:
	void updateChilds();
	void selectedTransporter(int i);

signals:
	void modified(bool);
	void closed();

protected:
	void closeEvent(QCloseEvent *e);

protected slots:
	void selectedSprite(int i);
	void pressedButton(int button,int x,int y);
	void updateCoordinates(int x,int y);
	void optionButton(int id);

private:
	ObjectListBox *sprite_list;
	SpritePreview *preview_sprite;
	MapGrid *map_area;
	Sprites **sprites;
	Obj::Type current_sprite;
	Q3ButtonGroup *optiongroup;
	CoordLabel *coord;
	MapEdit *map;

};

#endif							// !MAPEDITOR_H
