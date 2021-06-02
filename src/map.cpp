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

#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <qfile.h>
#include <qtextstream.h>

#include "krepton.h"

#include "map.h"


Map::Map(const QString &path)				// create from file
{
	qDebug() << "path='" << path << "'";

	QFile f(path);
	if (!f.open(QIODevice::ReadOnly))
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

        t.skipWhiteSpace();				// lose newline
	data = new Obj::Type[width*height];
	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			t.skipWhiteSpace();		// lose newline
			char c;
			t >> c;
			ref(x,y) = Obj::Type(c-'A');
		}
	}

	f.close();

	findStart();
	status.clear();					// map now OK
}


Map::Map(const Map &m)					// copy constructor
{
	qDebug() << "pw='" << m.password << "'";

	width = m.width; height = m.height;
	password = m.password;
	num_secs = m.num_secs;
	status = m.status;

	data = new Obj::Type[width*height];
	memcpy(data,m.data,width*height*sizeof(Obj::Type));
	findStart();

        for (TransporterList::const_iterator it = m.transporters.constBegin();
		it!=m.transporters.constEnd(); ++it)
	{
		transporters.append(new Transporter(**it));
	}
}


Map::Map(int sx,int sy,const QByteArray &pw)		// create with size
{
	qDebug() << "sx=" << sx << " sy=" << sy;

	width = sx; height = sy;
	password = pw;
	num_secs = 300;
	status = "";

	data = new Obj::Type[width*height];
	for (int i = (width*height-1); i>=0; --i) data[i] = Obj::Empty;
}


Map::~Map()
{
	qDebug() << "pw='" << password << "'";

	qDeleteAll(transporters);
	transporters.clear();
	delete[] data;
}



bool Map::save(const QString &path) const
{
	qDebug() << "pw='" << password << "' to='" << path << "'";

	QFile f(path);
	if (!f.open(QIODevice::WriteOnly))
	{
		reportError(ki18n("Cannot save map to '%1'"), path);
		return (false);
	}

	QTextStream t(&f);
	t << width << ' ' << height << ' ' << transporters.count() << ' ' << num_secs << '\n';
	t << password << '\n';

        for (TransporterList::const_iterator it = transporters.constBegin();
		it!=transporters.constEnd(); ++it)
	{
		const Transporter *tr = (*it);
		t << tr->orig_x << ' ' << tr->orig_y << ' '
		  << tr->dest_x << ' ' << tr->dest_y << '\n';
	}

	for (int y = 0; y<height; ++y)
	{
		for (int x = 0; x<width; ++x)
		{
			t << static_cast<char>(static_cast<int>(xy(x,y))+'A');
		}
		t << '\n';
	}

	f.close();
	return (true);
}


inline void Map::addTransporter(int ox,int oy,int dx,int dy)
{
	qDebug() << "oxy=" <<ox << "," << oy << " dxy=" << dx << "," << dy;
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


/* virtual */ bool Map::isempty(Obj::Type obj) const
{
	return (obj==Obj::Empty || obj==Obj::Ground1 || obj==Obj::Ground2);
}


/* virtual */ bool Map::isempty(int x,int y) const
{
	return (isempty(xy(x,y)));
}


/* static */ void Map::deltaForDirection(Orientation::Type dir, int *xp, int *yp)
{
	int xd = 0;
	int yd = 0;
	switch (dir)					// calculate displacement
	{
case Orientation::None:		         break;		// should never happen
case Orientation::North:	yd = -1; break;
case Orientation::East:		xd = +1; break;
case Orientation::South:	yd = +1; break;
case Orientation::West:		xd = -1; break;
	}

	*xp = xd;
	*yp = yd;
}


/* static */ char Map::showDirection(Orientation::Type dir)
{
	switch (dir)					// look at displacement
	{
case Orientation::North:	return ('N');
case Orientation::East:		return ('E');
case Orientation::South:	return ('S');
case Orientation::West:		return ('W');
default:			return ('?');
	}
}


// Check if the blip can go that way.
bool Map::blipTryDirection(const Monster *m, Orientation::Type dir) const
{
	int xd, yd;
	if (dir==Orientation::None) dir = m->orientation;
	deltaForDirection(dir, &xd, &yd);

	Obj::Type obj = xy(m->xpos+xd, m->ypos+yd);
	return (isempty(obj) || obj==Obj::Cage || obj==Obj::Repton);
}


// Send the blip that way, and take appropriate action.
bool Map::blipGoDirection(Monster *m, Orientation::Type dir)
{
	int xd, yd;
	if (dir==Orientation::None) dir = m->orientation;
	deltaForDirection(dir, &xd, &yd);

	Obj::Type obj = xy(m->xpos+xd, m->ypos+yd);
	if (isempty(obj))
	{
		m->xpos += xd;
		m->ypos += yd;
		m->orientation = dir;
		return (true);
	}

	if (obj==Obj::Cage || obj==Obj::Repton)
	{
		m->xpos += xd;
		m->ypos += yd;
		return (blipHit(m, obj));
	}

	return (false);
}


// Check and move the blip.
bool Map::updateBlip(Monster *m)
{
	const int mx = m->xpos;
	const int my = m->ypos;

	// is the spirit surrounded?
	if (!isempty(mx, my+1) && !isempty(mx+1, my) &&
	    !isempty(mx, my-1) && !isempty(mx-1, my))
	{
		qDebug() << "spirit surrounded at" << mx << my;
		return (false);				// nothing we can do
	}

	// is the spirit in empty space?
	if (isempty(mx, my+1) && isempty(mx+1, my) &&
	    isempty(mx, my-1) && isempty(mx-1, my) &&
	    isempty(mx-1, my-1) && isempty(mx-1, my+1) &&
	    isempty(mx+1, my-1) && isempty(mx+1, my+1))
	{
		qDebug() << "spirit confused at" << mx << my << showDirection(m->orientation);
		blipGoDirection(m);			// just go straight on
	}
	else
	{
		switch (m->orientation)
		{
case Orientation::North:
			if (!blipTryDirection(m) &&
			    blipTryDirection(m, Orientation::West) &&
			    blipTryDirection(m, Orientation::East) &&
			    isempty(mx-1, my+1))
			{
				qDebug() << "reorient at" << mx << my << "N, try E";
				if (blipGoDirection(m, Orientation::East)) break;
			}

			if (!blipGoDirection(m, Orientation::West) &&
			    !blipGoDirection(m))
			{
				m->orientation = Orientation::East;
				return (updateBlip(m));
			}
			break;

case Orientation::East:
			if (!blipTryDirection(m) &&
			    blipTryDirection(m, Orientation::North) &&
			    blipTryDirection(m, Orientation::South) &&
			    isempty(mx-1, my-1))
			{
				qDebug() << "reorient at" << mx << my << "E, try S";
				if (blipGoDirection(m, Orientation::South)) break;
			}

			if (!blipGoDirection(m, Orientation::North) &&
			    !blipGoDirection(m))
			{
				m->orientation = Orientation::South;
				return (updateBlip(m));
			}
			break;

case Orientation::South:
			if (!blipTryDirection(m) &&
			    blipTryDirection(m, Orientation::East) &&
			    blipTryDirection(m, Orientation::West) &&
			    isempty(mx+1, my-1))
			{
				qDebug() << "reorient at" << mx << my << "S, try W";
				if (blipGoDirection(m, Orientation::West)) break;
			}

			if (!blipGoDirection(m, Orientation::East) &&
			    !blipGoDirection(m))
			{
				m->orientation = Orientation::West;
				return (updateBlip(m));
			}
			break;

case Orientation::West:
			if (!blipTryDirection(m) &&
			    blipTryDirection(m, Orientation::South) &&
			    blipTryDirection(m, Orientation::North) &&
			    isempty(mx+1, my+1))
			{
				qDebug() << "reorient at" << mx << my << "W, try N";
				if (blipGoDirection(m, Orientation::North)) break;
			}

			if (!blipGoDirection(m, Orientation::South) &&
			    !blipGoDirection(m))
			{
				m->orientation = Orientation::North;
				return (updateBlip(m));
			}
			break;

case Orientation::None:
			break;				// should never happen
		}
	}

	if (m->xpos!=mx || m->ypos!=my)
		m->sprite = (m->sprite==Obj::Blip) ? Obj::Blip2 : Obj::Blip;
	return (true);					// because of animation
}


void Map::blipInitialDirection(Monster *m) const
{
	const int x = m->xpos;
	const int y = m->ypos;

	// TODO: mentioned somewhere, possibly at the Repton Resource Page
	// https://www.reptonresourcepage.co.uk/Walkthrough.php?game=2&plat=0&scene=Now&level=7
	// that the initial sprite orientation is different in the BBC and later
	// versions.  Check that the logic here is correct.
	if (!isempty(x-1,y) && isempty(x,y-1)) m->orientation = Orientation::North;
	else if (!isempty(x,y+1) && isempty(x-1,y)) m->orientation = Orientation::West;
	else if (!isempty(x,y-1) && isempty(x+1,y)) m->orientation = Orientation::East;
	else if (!isempty(x+1,y) && isempty(x,y+1)) m->orientation = Orientation::South;
}
