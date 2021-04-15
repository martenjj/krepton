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

#ifndef EPISODES_H
#define EPISODES_H

#include <qlist.h>

#include "map.h"

class Episode
{
public:
	Episode(const QString n,bool g,const QString p);
	const QString loadMaps(MapList *maps) const;
	bool saveInfoAndMaps(const MapList *maps) const;
	bool removeFiles() const;

	const QString &getName() const { return (name); }
    const QString getFilePath(const QString file = QString()) const;
	bool isGlobal() const { return (global); }

	static QString savePath(const QString &name);
	static QString sanitisedName(const QString &name);

private:
	QString name;
	QString path;
	bool global;
};


class EpisodeList : public QList<const Episode *>
{
public:
	static EpisodeList *list();
	const Episode *find(const QString &name);
	void add(const Episode *e);
	void remove(const Episode *e,bool noDelete = false);
	bool any() const;
	bool anyUser() const;

private:
	EpisodeList();
};

#endif							// !EPISODES_H
