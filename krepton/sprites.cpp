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

#include <klocale.h>

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qfile.h>
#include <qimage.h>
#include <qcolor.h>
#include <qregexp.h>
#include <qdir.h>

#include "krepton.h"
#include "episodes.h"

#include "sprites.h"


static const int whitethresh = 220;			// for recolouring previews

static const int xnum = 8;
static const int ynum = 6;


Sprites::Magnification Sprites::magnification = Sprites::Normal;
int Sprites::sprite_width = Sprites::base_width;
int Sprites::sprite_height = Sprites::base_height;
bool Sprites::magnificationset = false;


Sprites::Sprites()					// create as blank
{
	kdDebug(0) << k_funcinfo << endl;

	for (int y = 0; y<ynum; ++y)
	{
		for (int x = 0; x<xnum; ++x)
		{
			int i = y*xnum + x; 
			QPixmap px(base_width,base_height);
			rawsprites[i] = px;
		}
	}

        preparedFor = -1;				// nothing prepared yet
	multiRemoved = false;
	rawSet = false;
	status = QString::null;				// sprites now OK
}


Sprites::Sprites(const Episode *e)			// load from episode
{
	kdDebug(0) << k_funcinfo << "for '" << e->getName() << "'" << endl;

	status = QString("Not initialised");
	multiRemoved = false;
        preparedFor = -1;				// nothing prepared yet

        QDir d(e->getFilePath());
        d.setFilter(QDir::Files);
        d.setNameFilter("sprites[1-9]*.png;sprites.png;sprites.bmp");

        QRegExp rx("(\\d+)\\.");
        QStringList sfl = d.entryList();
        if (sfl.count()==0)
        {
            status = i18n("No sprite files found in %1").arg(d.absPath());
            return;
        }

        for (QStringList::const_iterator it = sfl.constBegin(); it!=sfl.constEnd(); ++it)
        {
            QString sf = (*it);

            int level = 0;				// assume all-levels sprites
            if (sf.find(rx)>-1)				// level-numbered file?
            {
                level = rx.cap(1).toInt();
            }

            kdDebug() << "  sprite file " << sf << " for level " << level << endl;

            QString path = d.filePath(sf);
            QPixmap allsprites(path);
            if (allsprites.isNull())
            {
		status = QString("%1, %2").arg(strerror(errno)).arg(path);
		return;
            }

            files[level] = allsprites;

            kdDebug(0) << "  -> serial " << allsprites.serialNumber() << endl;
        }

        if (files.isEmpty())				// should never happen
        {
            status = i18n("No sprites in map");
            return;
        }

	ensureRaw();
	status = QString::null;				// sprites now OK
}


void Sprites::ensureRaw()
{
    kdDebug(0) << k_funcinfo << endl;

    QPixmap std = files.constBegin().data();		// standard pixmaps for editor
    for (int y = 0; y<ynum; ++y)			// "0" will be first if present
    {
	for (int x = 0; x<xnum; ++x)
	{
	    int i = y*xnum + x; 

	    QPixmap px(base_width,base_height);
	    QPainter pp;

	    pp.begin(&px);
	    pp.drawPixmap(0,0,std,
			  x*base_width,y*base_height,
			  base_width,base_height);
	    pp.end();
	    rawsprites[i] = px;
	}
    }

    rawSet = true;
}


Sprites::Sprites(const Sprites &s)			// copy constructor
{
	kdDebug(0) << k_funcinfo << endl;

        files = s.files;				// implicit sharing
	for (int i = 0; i<Obj::num_sprites; ++i)	// copy explicitly
	{
		rawsprites[i] = s.rawsprites[i];
	}

        preparedFor = -1;				// nothing prepared yet
	multiRemoved = s.multiRemoved;
	rawSet= s.rawSet;
	status = QString::null;				// sprites now OK
}


bool Sprites::save(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

	if (!rawSet) ensureRaw();

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
	kdDebug(0) << "  save rawsprites to '" << path << "'" << endl;

	if (!allsprites.save(path,"PNG"))
	{
		reportError("Cannot save sprites to '%1'",path);
		return (false);
	}

	path = e->getFilePath("sprites.bmp");		// clean up obsolete file
	if (QFile::exists(path)) QFile::remove(path);

	if (multiRemoved)
	{
	    QDir d(e->getFilePath());
	    d.setFilter(QDir::Files);
	    d.setNameFilter("sprites[1-9]*.png");

	    QStringList sfl = d.entryList();
	    for (QStringList::const_iterator it = sfl.constBegin(); it!=sfl.constEnd(); ++it)
	    {
		QString sf = (*it);
		kdDebug(0) << "  removing level-specific '" << sf << "'" << endl;
		QFile::remove(d.filePath(sf));
	    }
	}
	else
	{
	    QValueList<int> levels = files.keys();
	    for (QValueList<int>::const_iterator it = levels.constBegin();
		 it!=levels.constEnd(); ++it)
	    {
		int level = (*it);
		if (level==0) continue;			// already saved above

		path = e->getFilePath(QString("sprites%1.png").arg(level));
		kdDebug(0) << "  save level " << level << " to '" << path << "'" << endl;

		if (!files[level].save(path,"PNG"))
		{
		    reportError("Cannot save sprites to '%1'",path);
		    return (false);
		}
	    }
	}

	return (true);
}


QPixmap Sprites::preview(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "for '" << e->getName() << "'" << endl;

	QPixmap preview;

	QString path;
	path = e->getFilePath("sprites.png");
	if (!QFile::exists(path)) path = e->getFilePath("sprites.bmp");
	if (!QFile::exists(path))
        {
            QDir d(e->getFilePath());
            d.setFilter(QDir::Files);
            d.setNameFilter("sprites*.png;sprites*.bmp");
            d.setSorting(QDir::Name);

            QStringList sfl = d.entryList();
            if (sfl.count()==0) return (preview);	// no sprite files at all!
            path = d.filePath(sfl.first());		// use first level file found
        }

	QPixmap allsprites(path);
	if (allsprites.isNull()) return (preview);

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


void Sprites::setPixel(Obj::Type obj,int x,int y,QColor colour,int level)
{
    QPainter p;

    if (level==0)					// normal operation
    {
	p.begin(&rawsprites[obj]);
	p.setPen(colour);
	p.drawPoint(x,y);

	rawSet = true;					// modified, so don't update
    }
    else						// set for level-specific
    {
	if (!files.contains(level))
	{
	    kdDebug(0) << k_funcinfo << "need new pixmap for level " << level << endl;
	    QPixmap levelpx(xnum*base_width,ynum*base_height);
	    levelpx.fill(Qt::black);
	    files[level] = levelpx;
	}

	p.begin(&files[level]);
	p.setPen(colour);

	int xb = obj % xnum;
	int yb = obj / xnum;
	p.drawPoint(xb*base_width+x,yb*base_height+y);

	preparedFor = -1;				// need update next time
    }

    p.end();
}


void Sprites::prepare(int level)
{
    kdDebug() << k_funcinfo << "level=" << level << " prep=" << preparedFor << endl;

    if (preparedFor==level) return;			// already ready for level?

    QPixmap src;					// source pixmap, if any

    if (files.contains(level))				// specific one present
    {
        src = files[level];				// pixmap for this level
        kdDebug() << "  prepare from pixmap " << src.serialNumber() << " for level " << level << endl;
    }
    else
    {
	kdDebug() << "  prepare from raw/edited" << endl;
    }

    for (int y = 0; y<ynum; ++y)
    {
	for (int x = 0; x<xnum; ++x)
	{
	    int i = y*xnum + x; 

	    QPixmap px(base_width,base_height);
	    if (!src.isNull())
	    {
                QPainter pp;

                pp.begin(&px);
                pp.drawPixmap(0,0,src,
                              x*base_width,y*base_height,
                              base_width,base_height);
                pp.end();
	    }
	    else px = rawsprites[i];

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
	}
    }

    preparedFor = level;
    kdDebug() << "  prepared for level " << preparedFor << endl;
}


bool Sprites::hasMultiLevels() const
{
    kdDebug() << k_funcinfo << "c0=" << files.contains(0) << " count=" << files.count() << endl;
    return (files.count()>=2 || (!files.contains(0) && files.count()>=1));
}


void Sprites::removeMultiLevels()
{
    kdDebug() << k_funcinfo << endl;
    files.clear();					// drastic, but what we need
    multiRemoved = true;				// note for save later
}
