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

#ifndef SPRITES_H
#define SPRITES_H

#include <qcolor.h>
#include <qpixmap.h>

class Episode;

class Sprites
{
public:
	static const int base_width = 32;
	static const int base_height = 32;
	static int sprite_width;
	static int sprite_height;

	enum Magnification { Half=0, Normal=1, Double=2 };

	Sprites(const Episode *e);			// load from episode
	Sprites(const Sprites &s);			// copy constructor
	const QString loadStatus() const { return (status); }

	QPixmap get(Obj::Type obj) const { return (sprites[obj]); }
	QPixmap getRaw(Obj::Type obj) const { return (rawsprites[obj]); }

	void setPixel(Obj::Type obj,int x,int y,QColor colour);
	bool save(const Episode *e) const;

	static QPixmap preview(const Episode *e);
	static void setMagnification(Sprites::Magnification mag);
	static Sprites::Magnification getMagnification() { return (magnification); }

private:
	static const int xnum = 8;
	static const int ynum = 6;
	static Sprites::Magnification magnification;
	static bool magnificationset;

	void update();

	QPixmap sprites[Obj::num_sprites];		// scaled & masked, for game
	QPixmap rawsprites[Obj::num_sprites];		// unprocessed, for editor
	bool edited[Obj::num_sprites];			// updated in editor
	QString status;
};

#endif							// !SPRITES_H
