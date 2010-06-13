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

#include <kmessagebox.h>

#include <qstringlist.h>

#include "krepton.h"
#include "map.h"

#include "checkmap.h"


inline const QString CheckMap::displayPos(int x,int y)
{
	return (QString("[X:%1,Y:%2]").arg(x+1).arg(y+1));
}


inline void CheckMap::addItem(const QString where,const QString what,bool warning)
{
	addItem(where+(warning ? " (warning)" : "")+": "+what);
}


void CheckMap::addItem(const QString msg)
{
	msgs.append(msg);

	CheckMap::Severity thissev = CheckMap::Fatal;
	if (msg.contains("(warning)")) thissev = CheckMap::Warning;
	severity = qMax(severity, thissev);
}


void CheckMap::strictCheckTransporter(const QString maploc,const Map *mm,int x,int y)
{
	int found = 0;

	Q3PtrListIterator<Transporter> ti(mm->transporters);
	for (const Transporter *tt; (tt = ti.current())!=NULL; ++ti)
	{
		if (tt->orig_x==x && tt->orig_y==y) ++found;
	}

	if (found==1) return;				// unique, all good

	const QString transloc = QString("%1 at %2").arg(maploc).arg(displayPos(x,y));
	if (found==0) addItem(transloc,"transporter with no origin");
	else addItem(transloc,"more than one transporter origin");
}


void CheckMap::strictCheckTransporters(const QString maploc,const Map *mm)
{
	Q3PtrListIterator<Transporter> ti(mm->transporters);
	const Transporter *tt;
	for (int thistrans = 1; (tt = ti.current())!=NULL; ++ti,++thistrans)
	{
		const QString transloc = QString("%1 transporter #%2").arg(maploc).arg(thistrans);

		if (tt->orig_x<0 || tt->orig_x>=mm->width || tt->orig_y<0 || tt->orig_y>=mm->height)
		{					// Avoid out-of-range
			addItem(transloc,QString("origin at %1 is out of range").arg(displayPos(tt->orig_x,tt->orig_y)));
			return;
		}
		if (tt->dest_x<0 || tt->dest_x>=mm->width || tt->dest_y<0 || tt->dest_y>=mm->height)
		{					// Avoid out-of-range
			addItem(transloc,QString("destination at %1 is out of range").arg(displayPos(tt->dest_x,tt->dest_y)));
			return;
		}

		if (mm->xy(tt->orig_x,tt->orig_y)!=Obj::Transport)
			addItem(transloc,QString("origin at %1 is not a transporter").arg(displayPos(tt->orig_x,tt->orig_y)));
		if (mm->xy(tt->dest_x,tt->dest_y)!=Obj::Empty &&
		    mm->xy(tt->dest_x,tt->dest_y)!=Obj::Repton)
			addItem(transloc,QString("destination at %1 is not empty").arg(displayPos(tt->dest_x,tt->dest_y)),true);
	}
}



CheckMap::CheckMap(const MapList maps)
{
	kDebug();

	msgs.clear();
	severity = CheckMap::Ok;

	if (maps.count()<1)				// check something to check!
	{
		addItem("General","no levels");
		return;
	}

	Q3PtrListIterator<Map> mi(maps);
	const Map *mm;
	int thismap;

	for (thismap = 1; (mm = mi.current())!=NULL; ++mi,++thismap)
	{
		const QString maploc = QString("Map #%1").arg(thismap);

		if (mm->password=="")
			addItem(maploc,"no password specified");
 		if (mm->num_secs<=0)
			addItem(maploc,QString("invalid time limit %1").arg(mm->num_secs));

		int num_reptons = 0;
		int num_diamonds = 0;
		int num_blips = 0;
		int num_cages = 0;
		int num_bombs = 0;
		int num_strongboxes = 0;
		int num_keys = 0;
		int num_rocks = 0;
		int num_eggs = 0;
		int num_crowns = 0;

		QString rt;
		for (int y = 0; y<mm->height; ++y)
		{
			for (int x = 0; x<mm->width; ++x)
			{
				switch (mm->xy(x,y))
				{
case Obj::Repton:			++num_reptons;
					break;

case Obj::Diamond:			++num_diamonds;
					break;

case Obj::Blip:				++num_blips;
					break;

case Obj::Cage:				++num_cages;
					break;

case Obj::Bomb:				++num_bombs;
					break;

case Obj::Strongbox:			++num_strongboxes;
					break;

case Obj::Key:				++num_keys;
					break;

case Obj::Rock:				++num_rocks;
					break;

case Obj::Egg:				++num_eggs;
					break;

case Obj::Crown:			++num_crowns;
					break;

case Obj::Transport:			strictCheckTransporter(maploc,mm,x,y);
					break;

default:				;		// Avoid warnings
				}
			}
		}

 		if (num_reptons==0)
			addItem(maploc,"no Repton start position");
 		if (num_reptons>1)
			addItem(maploc,"more than one Repton start position");
 		if (num_bombs==0)
			addItem(maploc,"no end level object");
 		if (num_crowns==0)
			addItem(maploc,QString("no bonus object"),true);
 		if (num_cages>num_blips)
			addItem(maploc,QString("has %1 cages but only %2 spirits").arg(num_cages).arg(num_blips));
 		if (num_cages<num_blips)
			addItem(maploc,QString("has %1 spirits but only %2 cages").arg(num_blips).arg(num_cages),true);
 		if (num_strongboxes>0 && num_keys==0)
			addItem(maploc,QString("has %1 strongboxes but no keys").arg(num_strongboxes));
		if (num_eggs>0 && num_rocks==0)
			addItem(maploc,QString("has %1 eggs but no rocks").arg(num_eggs),true);

		strictCheckTransporters(maploc,mm);
	}

	// Check for duplicated passwords
	QStringList pwds;

	for (mi.toFirst(); (mm = mi.current())!=NULL; ++mi) pwds.append(mm->password);
	for (mi.toFirst(),thismap = 1; (mm = mi.current())!=NULL; ++mi,++thismap)
	{
            int idx = pwds.indexOf(mm->password, Qt::CaseInsensitive);
            if (idx>thismap)
            {
                    const QString pwloc = QString("Maps #%1 and #%2").arg(thismap).arg(idx);
                    addItem(pwloc,"have the same password");
            }
	}

	kDebug() << "done sev=" << severity;
}


const QString CheckMap::detail(const QString s1,const QString s2)
{
	return (msgs.empty() ? QString::null : s1+msgs.join(s2+s1)+s2);
}


int CheckMap::report(QWidget *parent,const QString quest,bool positive)
{
	if (severity==CheckMap::Ok)
	{
		if (positive) KMessageBox::information(parent,"The consistency check was successful");
		return (0);
	}

	QString s = "<qt>The consistency check failed for these reasons:";
	s += "<ul>"+detail()+"</ul>";

	if (severity==CheckMap::Fatal && quest!=QString::null)
	{
		s += QString("%1.").arg(quest);
		return (KMessageBox::warningYesNo(parent,s));
	}

	KMessageBox::sorry(parent,s);
	return (0);
}
