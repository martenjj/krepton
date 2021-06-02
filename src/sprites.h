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
#include <qmap.h>


class Episode;


class Sprites
{
public:
	static const int base_width = 32;
	static const int base_height = 32;
	static int sprite_width;
	static int sprite_height;

	enum Magnification
	{
		Unset = -1,
		Half = 0,
		Normal = 1,
		Double = 2
	};

	Sprites();					// create as blank
	explicit Sprites(const Episode *e);		// load from episode
	explicit Sprites(const Sprites *s);		// create a copy
	explicit Sprites(const Sprites &s) = delete;	// copy constructor
	~Sprites() = default;

	const QString &loadStatus() const			{ return (status); }

	void prepare(int level);
	const QPixmap &get(Obj::Type obj, bool paused = false) const	{ Q_ASSERT(!sprites.isEmpty()); return (!paused ? sprites.at(obj) : greysprites.at(obj)); }
	const QPixmap &getRaw(Obj::Type obj) const			{ Q_ASSERT(!rawsprites.isEmpty()); return (rawsprites.at(obj)); }

	void setPixel(Obj::Type obj,int x,int y,QColor colour,int level = 0);
	bool save(const Episode *e);

	bool hasMultiLevels() const;
	void removeMultiLevels();

	static QPixmap preview(const Episode *e);
	static void setMagnification(Sprites::Magnification mag);
	static Sprites::Magnification getMagnification()	{ return (magnification); }

private:
	static Sprites::Magnification magnification;

	void ensureRaw();

	QMap<int,QPixmap> files;			// source files per-level
	QVector<QPixmap> sprites;			// scaled & masked, for game
	QVector<QPixmap> greysprites;			// grey scaled, for paused game
	QVector<QPixmap> rawsprites;			// unprocessed, for editor

	int preparedFor;				// level sprites are ready for
	bool multiRemoved;				// remove levels on saving

	QString status;					// file loading status
};

#endif							// !SPRITES_H
