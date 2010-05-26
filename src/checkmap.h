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

#ifndef CHECKMAP_H
#define CHECKMAP_H

#include <qstringlist.h>

#include "map.h"

class CheckMap
{
public:
	enum Severity { Ok, Warning, Fatal };

	CheckMap(const MapList maps);
	CheckMap::Severity status() { return (severity); }
	const QString detail(const QString s1 = "<li>",const QString s2 = "</li>");
	int report(QWidget *parent = NULL,const QString quest = QString::null,bool positive = false);

private:
	CheckMap::Severity severity;
	QStringList msgs;

	static const QString displayPos(int x,int y);
	void addItem(const QString msg);
	void addItem(const QString where,const QString what,bool warning = false);
	void strictCheckTransporter(const QString loc,const Map *map,int x,int y);
	void strictCheckTransporters(const QString loc,const Map *map);
};

#endif							// !CHECKMAP_H
