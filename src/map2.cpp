////////////////////////////////////////////////////////////////////////////
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

#include "krepton.h"

#include "map2.h"


MapEdit::MapEdit(const Map &m) : Map(m)			// copy constructor
{
	qDebug() << "pw='" << getPassword() << "'";
}

MapEdit::MapEdit(int sx,int sy,const QByteArray &pw) : Map(sx,sy,pw)
{
	qDebug() << "sx=" << sx << " sy=" << sy;
}


MapEdit::~MapEdit()
{
	qDebug() << "pw='" << getPassword() << "'";
}

TransporterList MapEdit::getTransportersList()
{
	return (transporters);
}

void MapEdit::transporterInsert(int ox,int oy,int dx,int dy)
{
	if (ox<1 || oy<1 || dx<1 || dy<1 ||		// shouldn't happen, spinboxes enforce
	    ox>width || oy>height ||
	    dx>width || dy>height) return;

	Transporter *t = new Transporter(ox-1,oy-1,dx-1,dy-1);
	transporters.append(t);
}


void MapEdit::transporterGet(int item,int *ox,int *oy,int *dx,int *dy)
{
	if (item>=transporters.size()) return;

	const Transporter *t = transporters.at(item);

	if (ox!=NULL) *ox = t->orig_x+1;
	if (ox!=NULL) *oy = t->orig_y+1;
	if (dx!=NULL) *dx = t->dest_x+1;
	if (dy!=NULL) *dy = t->dest_y+1;
}


void MapEdit::transporterChange(int item,int ox,int oy,int dx,int dy)
{
	if (ox<1 || oy<1 || dx<1 || dy<1 ||		// shouldn't happen, spinboxes enforce
	    ox>width || oy>height ||
	    dx>width || dy>height) return;

	Transporter *t = transporters.at(item);

	t->orig_x = ox-1;
	t->orig_y = oy-1;
	t->dest_x = dx-1;
	t->dest_y = dy-1;
}


void MapEdit::transporterRemove(int item)
{
	if (transporters.count()<1) return;		// shouldn't happen, GUI disables
	if (item>=static_cast<int>(transporters.count())) return;

	Transporter *t = transporters.at(item);
	transporters.removeAt(item);
	delete t;
}


void MapEdit::changePassword(const QByteArray &pw)
{
	password = pw;
}


void MapEdit::changeTime(int t)
{
	num_secs = t;
}



void MapEditList::mapInsert(int sx,int sy,const QByteArray &password)
{
	MapEdit *m = new MapEdit(sx,sy,password);
	append(m);
}


void MapEditList::mapRemove(int item)
{
	MapEdit *m = at(item);
	removeAt(item);
	delete m;
}


void MapEditList::mapMoveUp(int item)
{
	if (item<=0) return;				// shouldn't happen, GUI disables

	MapEdit *m = at(item);
	removeAt(item);
	insert(item-1,m);
}


void MapEditList::mapMoveDown(int item)
{
	int last_item = count()-1;
	if (item>=last_item) return;			// shouldn't happen, GUI disables

	MapEdit *m = at(item);
	removeAt(item);
	insert(item+1,m);
}


MapEditList::operator MapList()
{
	qDebug();

	MapList ml;
	for (MapEditList::const_iterator it = constBegin();
		it!=constEnd(); ++it)
	{
		const Map *mm = (*it);
		ml.append(const_cast<Map *>(mm));
	}

	qDebug() << "done count=" << ml.count();
	return (ml);
}
