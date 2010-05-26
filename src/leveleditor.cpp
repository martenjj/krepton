////////////////////////////////////////////////////////////////////////////
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

#include "config.h"

#include <q3ptrlist.h>

#include "krepton.h"
#include "map.h"

#include "leveleditor.h"


Q3PtrList<Transporter> LevelEditor::getTransportersList(const Map *mm)
{
	return (mm->transporters);
}

void LevelEditor::mapInsert(MapList *ml,int sx,int sy,const QString password)
{
	Map *m = new Map(sx,sy,password);
	ml->append(m);
}


void LevelEditor::mapRemove(MapList *ml,int item)
{
	Map *m = ml->at(item);
	ml->remove(item);
	delete m;
}


void LevelEditor::mapMoveUp(MapList *ml,int item)
{
	if (item<=0) return;				// shouldn't happen, GUI disables

	Map *m = ml->at(item);
	ml->remove(item);
	ml->insert(item-1,m);
}


void LevelEditor::mapMoveDown(MapList *ml,int item)
{
	int last_item = ml->count()-1;
	if (item>=last_item) return;			// shouldn't happen, GUI disables

	Map *m = ml->at(item);
	ml->remove(item);
	ml->insert(item+1,m);
}


void LevelEditor::transporterInsert(Map *mm,int ox,int oy,int dx,int dy)
{
	if (ox<1 || oy<1 || dx<1 || dy<1 ||		// shouldn't happen, spinboxes enforce
	    ox>mm->width || oy>mm->height ||
	    dx>mm->width || dy>mm->height) return;

	Transporter *t = new Transporter(ox-1,oy-1,dx-1,dy-1);
	mm->transporters.append(t);
}


void LevelEditor::transporterGet(Map *mm,int item,int *ox,int *oy,int *dx,int *dy)
{
	const Transporter *t = mm->transporters.at(item);

	*ox = t->orig_x+1;
	*oy = t->orig_y+1;
	*dx = t->dest_x+1;
	*dy = t->dest_y+1;
}


void LevelEditor::transporterChange(Map *mm,int item,int ox,int oy,int dx,int dy)
{
	if (ox<1 || oy<1 || dx<1 || dy<1 ||		// shouldn't happen, spinboxes enforce
	    ox>mm->width || oy>mm->height ||
	    dx>mm->width || dy>mm->height) return;

	Transporter *t = mm->transporters.at(item);

	t->orig_x = ox-1;
	t->orig_y = oy-1;
	t->dest_x = dx-1;
	t->dest_y = dy-1;
}


void LevelEditor::transporterRemove(Map *mm,int item)
{
	if (mm->transporters.count()<1) return;		// shouldn't happen, GUI disables
	if (item>=((int) mm->transporters.count())) return;

	Transporter *t = mm->transporters.at(item);
	mm->transporters.remove(item);
	delete t;
}


void LevelEditor::changePassword(Map *mm,const QString &pw)
{
	mm->password = pw;
}


void LevelEditor::changeTime(Map *mm,int t)
{
	mm->num_secs = t;
}
