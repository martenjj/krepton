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

#include "config.h"

#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <qfile.h>
#include <qtstream.h>

#include "krepton.h"

#include "map.h"


Map::Map(const QString &path)				// create from file
{
	kdDebug(0) << k_funcinfo << "path='" << path << "'" << endl;

	QFile f(path);
	if (!f.open(IO_ReadOnly))
	{
		status = QString("%1, %2").arg(strerror(errno)).arg(path);
		return;
	}

	QTextStream t(&f);
	int num_transporters;
	t >> width >> height >> num_transporters >> num_secs;
	t >> password;

	while (num_transporters--)
	{
		int ox, oy, dx, dy;
		t >> ox >> oy >> dx >> dy;
		addTransporter(ox,oy,dx,dy);
	}

	data = new Obj::Type[width*height];
	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			char c;
			t >> c;
			ref(x,y) = Obj::Type(c-'A');
		}
	}

	f.close();

	findStart();
	status = QString::null;				// map now OK
}


Map::Map(const Map &m)					// copy constructor
{
	kdDebug(0) << k_funcinfo << "pw='" << m.password << "'" << endl;

	width = m.width; height = m.height;
	password = m.password;
	num_secs = m.num_secs;
	status = m.status;

	data = new Obj::Type[width*height];
	memcpy(data,m.data,width*height*sizeof(Obj::Type));
	findStart();

	QPtrListIterator<Transporter> ti(m.transporters);
	for (const Transporter *tt; (tt = ti.current())!=NULL; ++ti)
	{
		transporters.append(new Transporter(*tt));
	}

//	transporters.setAutoDelete(true);
//	kdDebug(0) << k_funcinfo << "done" << endl;
}


Map::Map(int sx,int sy,const QString pw)		// create with size
{
	kdDebug(0) << k_funcinfo << "sx=" << sx << " sy=" << sy << endl;

	width = sx; height = sy;
	password = pw;
	num_secs = 300;
	status = QString::null;

	data = new Obj::Type[width*height];
	for (int i = (width*height-1); i>=0; --i) data[i] = Obj::Empty;

//	transporters.setAutoDelete(true);
//	kdDebug(0) << k_funcinfo << "done" << endl;
}


Map::~Map()
{
	kdDebug(0) << k_funcinfo << "pw='" << password << "'" << endl;

	transporters.setAutoDelete(true);
	transporters.clear();
	delete[] data;
}



bool Map::save(const QString &path) const
{
	kdDebug(0) << k_funcinfo << "pw='" << password << "' to='" << path << "'" << endl;

	QFile f(path);
	if (!f.open(IO_WriteOnly))
	{
		reportError("Cannot save map to '%1'",path);
		return (false);
	}

	QTextStream t(&f);
	t << width << ' ' << height << ' ' << transporters.count() << ' ' << num_secs << '\n';
	t << password << '\n';

	QPtrListIterator<Transporter> ti(transporters);
	const Transporter *tr;
	while ((tr = ti.current())!=NULL)
	{
		t << tr->orig_x << ' ' << tr->orig_y << ' '
		  << tr->dest_x << ' ' << tr->dest_y << '\n';
		++ti;
	}

	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			t << (char) (((int) xy(x,y))+'A');
		}
		t << '\n';
	}

	f.close();
	return (true);
}


inline void Map::addTransporter(int ox,int oy,int dx,int dy)
{
	kdDebug(0) << k_funcinfo << "oxy=" <<ox << "," << oy << " dxy=" << dx << "," << dy << endl;
	transporters.append(new Transporter(ox,oy,dx,dy));
}


void Map::findStart()
{
	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			if (ref(x,y)==Obj::Repton)
			{
				xstart = x;
				ystart = y;
				return;
			}
		}
	}
}
