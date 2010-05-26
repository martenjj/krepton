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

#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <qptrlist.h>
#include <qstring.h>

#include "map.h"

class LevelEditor
{
public:
	static QPtrList<Transporter> getTransportersList(const Map *mm);
	static void mapInsert(MapList *ml,int sx,int sy,const QString password);
	static void mapRemove(MapList *ml,int item);
	static void mapMoveUp(MapList *ml,int item);
	static void mapMoveDown(MapList *ml,int item);
	static void transporterInsert(Map *mm,int ox,int oy,int dx,int dy);
	static void transporterGet(Map *mm,int item,int *ox,int *oy,int *dx,int *dy);
	static void transporterChange(Map *mm,int item,int ox,int oy,int dx,int dy);
	static void transporterRemove(Map *mm,int item);
	static void changePassword(Map *mm,const QString &pw);
	static void changeTime(Map *mm,int t);
};

#endif							// !LEVELEDITOR_H
