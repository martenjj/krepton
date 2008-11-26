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

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qfile.h>
#include <qimage.h>
#include <qcolor.h>

#include "krepton.h"
#include "episodes.h"

#include "sprites.h"


static const int whitethresh = 220;			// for recolouring previews


Sprites::Magnification Sprites::magnification = Sprites::Normal;
int Sprites::sprite_width = Sprites::base_width;
int Sprites::sprite_height = Sprites::base_height;
bool Sprites::magnificationset = false;

Sprites::Sprites(const Episode *e)			// load from episode
{
	kdDebug(0) << k_funcinfo << "for '" << e->getName() << "'" << endl;

	status = QString("Not initialised");

	QString path;
	path = e->getFilePath("sprites.png");
	if (!QFile::exists(path)) path = e->getFilePath("sprites.bmp");
	if (!QFile::exists(path))
	{
		status = QString("%1, %2").arg(strerror(ENOENT)).arg(path);
		return;
	}

	QPixmap allsprites(path);
	if (allsprites.isNull())
	{
		status = QString("%1, %2").arg(strerror(errno)).arg(path);
		return;
	}
	kdDebug(0) << k_funcinfo << "from '" << path << "'" << endl;

	for (int y = 0; y<ynum; ++y)
	{
		for (int x = 0; x<xnum; ++x)
		{
			int i = y*xnum + x; 

			QPixmap px(base_width,base_height);
			QPainter pp;

			pp.begin(&px);
			pp.drawPixmap(0,0,allsprites,
				     x*base_width,y*base_height,
				     base_width,base_height);
			pp.end();

			rawsprites[i] = px;
			edited[i] = true;		// force update first time
		}
	}


	this->update();
	status = QString::null;				// sprites now OK
}


Sprites::Sprites(const Sprites &s)			// copy constructor
{
	kdDebug(0) << k_funcinfo << endl;
	for (int i = 0; i<Obj::num_sprites; ++i)
	{
		sprites[i] = s.sprites[i];
	}

	for (int i = 0; i<Obj::num_sprites; ++i)
	{
		rawsprites[i] = s.rawsprites[i];
	}

	for (int i = 0; i<Obj::num_sprites; ++i)
	{
		edited[i] = s.edited[i];
	}

	this->update();
	kdDebug(0) << k_funcinfo << "done" << endl;
}


bool Sprites::save(const Episode *e) const
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

	QPixmap allsprites(xnum*base_width,ynum*base_height);

	QPainter p;
	p.begin(&allsprites);
	for (int y = 0; y<ynum; ++y)
	{
		for (int x = 0; x<xnum; ++x)
		{
			p.drawPixmap(x*base_width,y*base_height,
				     rawsprites[y*xnum+x]);
		}
	}
	p.end();

	QString path = e->getFilePath("sprites.png");
	kdDebug(0) << k_funcinfo << "save to '" << path << "'" << endl;

	if (!allsprites.save(path,"PNG"))
	{
		reportError("Cannot save sprites to '%1'",path);
		return (false);
	}

	path = e->getFilePath("sprites.bmp");		// clean up obsolete file
	if (QFile::exists(path)) QFile::remove(path);

	return (true);
}


QPixmap Sprites::preview(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "for '" << e->getName() << "'" << endl;

	QPixmap preview;

	QString path;
	path = e->getFilePath("sprites.png");
	if (!QFile::exists(path)) path = e->getFilePath("sprites.bmp");
	if (!QFile::exists(path)) return (preview);

	QPixmap allsprites(path);
	if (allsprites.isNull()) return (preview);

//	kdDebug(0) << k_funcinfo << "from '" << path << "'" << endl;

	preview.resize(base_width,base_height);

	int y = Obj::Repton / xnum;
	int x = Obj::Repton % xnum;

	QPainter p(&preview);
	p.drawPixmap(0,0,allsprites,
		     x*base_width,y*base_height,
		     base_width,base_height);

	QImage pi(preview.convertToImage());
	for (int x = 0; x<pi.width(); ++x)
	{
		for (int y = 0; y<pi.height(); ++y)
		{
			QRgb pix = pi.pixel(x,y);
			if (!(qRed(pix)>whitethresh &&
			      qGreen(pix)>whitethresh &&
			      qBlue(pix)>whitethresh)) continue;
			pi.setPixel(x,y,qRgb(whitethresh,whitethresh,whitethresh));
		}
	}
	preview.convertFromImage(pi,0);

	preview.setMask(preview.createHeuristicMask());
	return (preview);
}

void Sprites::setMagnification(Sprites::Magnification mag)
{
	kdDebug(0) << k_funcinfo << "mag=" << mag << " set=" << magnificationset << endl;

	if (magnificationset) return;
	magnification = mag;
	magnificationset = true;

	sprite_width = base_width*(1<<(((int) magnification)))/2;
	sprite_height = base_height*(1<<(((int) magnification)))/2;
}


void Sprites::setPixel(Obj::Type obj,int x,int y,QColor colour)
{
	kdDebug(0) << k_funcinfo << "x=" << x << " y=" << y << " c=" << endl;

	QPainter p;

	p.begin(&rawsprites[obj]);
	p.setPen(colour);
	p.drawLine(x,y,x,y);
	p.end();

	edited[obj] = true;
}


void Sprites::update()
{
	for (int y = 0; y<ynum; ++y)
	{
		for (int x = 0; x<xnum; ++x)
		{
			int i = y*xnum + x; 
			if (!edited[i]) continue;

//			kdDebug(0) << k_funcinfo << "i=" << i << endl;

			QPixmap px = rawsprites[i];
			if (magnification!=Sprites::Normal)
			{
				QPixmap px2(sprite_width,sprite_height);
				QPainter pp2(&px2);
				pp2.drawPixmap(px2.rect(),px);
				px = px2;
			}

			if (i==Obj::Blip || i==Obj::Blip2)
			{
				px.setMask(px.createHeuristicMask());
			}

			sprites[i] = px;
			edited[i] = false;
		}
	}
}
