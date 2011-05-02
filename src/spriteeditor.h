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

#ifndef SPRITEEDITOR_H
#define SPRITEEDITOR_H

#include <qwidget.h>

#include "krepton.h"

class ObjectListBox;
class SpritePreview;
class SpriteGrid;
class KColorButton;
class Sprites;

class SpriteEditor : public QWidget
{
	Q_OBJECT

public:
	SpriteEditor(QWidget *parent,Sprites **ss);

public slots:
	void updateChilds();

signals:
	void changedSprite();
	void coordinatePosition(int x,int y);

protected slots:
	void selectedSprite(int i);
	void pressedButton(int button,int x,int y);

private:
	ObjectListBox *sprite_list;
	KColorButton *left_color;
	KColorButton *right_color;
        SpritePreview *preview_sprite;
	SpriteGrid *grid_sprite;
	Sprites **sprites;
	Obj::Type current_sprite;
};

#endif							// !SPRITEEDITOR_H
