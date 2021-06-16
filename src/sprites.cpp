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
#include "sprites.h"

#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qfile.h>
#include <qregexp.h>
#include <qdir.h>
#include <qelapsedtimer.h>

#include "episodes.h"


static const int whitethresh = 220;			// for recolouring previews

static const int xnum = 8;
static const int ynum = 6;


Sprites::Magnification Sprites::magnification = Sprites::Unset;
int Sprites::sprite_width = Sprites::base_width;
int Sprites::sprite_height = Sprites::base_height;


Sprites::Sprites()					// create as blank
{
	qDebug() << "empty";
	Q_ASSERT((xnum*ynum)==Obj::num_sprites);	// check size consistent here

	preparedFor = -1;				// nothing prepared yet
	multiRemoved = false;
	status.clear();					// sprites now OK
}


Sprites::Sprites(const Episode *e)			// load from episode
{
	qDebug() << "from" << e->getName();

	status = i18n("Not initialised");
	multiRemoved = false;
	preparedFor = -1;				// nothing prepared yet

        QDir d(e->getFilePath());
        d.setFilter(QDir::Files);
        d.setNameFilters(QString("sprites[1-9]*.png;sprites.png;sprites.bmp").split(';'));

	QRegExp rx("(\\d+)\\.");
        const QStringList sfl = d.entryList();
        if (sfl.isEmpty())
        {
            status = i18n("No sprite files found in '%1'", d.absolutePath());
            return;
        }

        for (const QString &sf : sfl)
        {
            int level = 0;				// assume all-levels sprites
            if (sf.contains(rx))			// level-numbered file?
            {
                level = rx.cap(1).toInt();
            }
            qDebug() << "  sprite file" << sf << "for level" << level;

            const QString path = d.filePath(sf);
            const QPixmap allsprites(path);
            if (allsprites.isNull())
            {
		status = i18n("%1, %2", strerror(errno), path);
		return;
            }

            files[level] = allsprites;
        }

        if (files.isEmpty())				// should never happen
        {
            status = i18n("No sprites in map");
            return;
        }

	ensureRaw();
	status.clear();					// sprites now OK
}


Sprites::Sprites(const Sprites *s)
{
	qDebug() << "copy";

	files = s->files;				// implicit sharing
	rawsprites = s->rawsprites;			// implicit sharing

	preparedFor = -1;				// nothing prepared yet
	multiRemoved = s->multiRemoved;
	status.clear();                                 // sprites now OK
}


void Sprites::ensureRaw()
{
    qDebug();

    rawsprites.clear();
    rawsprites.resize(Obj::num_sprites);

    const QPixmap &std = files.constBegin().value();	// standard pixmaps for editor
    for (int y = 0; y<ynum; ++y)			// "0" will be first if present
    {
	for (int x = 0; x<xnum; ++x)
	{
	    int i = y*xnum+x;
	    rawsprites[i] = std.copy(x*base_width, y*base_height, base_width, base_height);
	}
    }
}


bool Sprites::save(const Episode *e)
{
	qDebug() << "name" << e->getName();

	if (rawsprites.isEmpty()) ensureRaw();

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
	qDebug() << "  save rawsprites to" << path;

	if (!allsprites.save(path,"PNG"))
	{
		reportError(ki18n("Cannot save sprites to '%1'"), path);
		return (false);
	}

	path = e->getFilePath("sprites.bmp");		// clean up obsolete file
	if (QFile::exists(path)) QFile::remove(path);

	if (multiRemoved)
	{
	    QDir d(e->getFilePath());
	    d.setFilter(QDir::Files);
	    d.setNameFilters(QStringList("sprites[1-9]*.png"));

	    QStringList sfl = d.entryList();
	    for (QStringList::const_iterator it = sfl.constBegin(); it!=sfl.constEnd(); ++it)
	    {
		QString sf = (*it);
		qDebug() << "  removing level-specific" << sf;
		QFile::remove(d.filePath(sf));
	    }
	}
	else
	{
	    QList<int> levels = files.keys();
	    for (QList<int>::const_iterator it = levels.constBegin();
		 it!=levels.constEnd(); ++it)
	    {
		int level = (*it);
		if (level==0) continue;			// already saved above

		path = e->getFilePath(QString("sprites%1.png").arg(level));
		qDebug() << "  save level" << level << "to" << path;

		if (!files[level].save(path,"PNG"))
		{
		    reportError(ki18n("Cannot save sprites to '%1'"), path);
		    return (false);
		}
	    }
	}

	return (true);
}


QPixmap Sprites::preview(const Episode *e)
{
	qDebug() << "for" << e->getName();

	QString path;
	path = e->getFilePath("sprites.png");
	if (!QFile::exists(path)) path = e->getFilePath("sprites.bmp");
	if (!QFile::exists(path))
        {
            QDir d(e->getFilePath());
            d.setFilter(QDir::Files);
            d.setNameFilters(QString("sprites*.png;sprites*.bmp").split(';'));
            d.setSorting(QDir::Name);

            const QStringList sfl = d.entryList();
            if (sfl.isEmpty()) return (QPixmap());	// no sprite files at all!
            path = d.filePath(sfl.first());		// use first level file found
        }

	const QPixmap allsprites(path);
	if (allsprites.isNull()) return (QPixmap());

	int y = Obj::Repton / xnum;
	int x = Obj::Repton % xnum;
	QPixmap preview = allsprites.copy(x*base_width,y*base_height,
                                          base_width,base_height);
	preview.setMask(preview.createHeuristicMask());
	return (preview);
}


void Sprites::setMagnification(Sprites::Magnification mag)
{
	qDebug() << "cur" << magnification << "->" << mag;

	if (magnification!=Sprites::Unset)
	{
		qWarning() << "Magnification already set to" << magnification;
		return;
	}

	magnification = mag;
	sprite_width = base_width*(1<<((static_cast<int>(magnification))))/2;
	sprite_height = base_height*(1<<((static_cast<int>(magnification))))/2;
}


void Sprites::setPixel(Obj::Type obj,int x,int y,QColor colour,int level)
{
    QPainter p;

    if (level==0)					// normal operation
    {
	if (rawsprites.isEmpty()) rawsprites.resize(Obj::num_sprites);

	p.begin(&rawsprites[obj]);
	p.setPen(colour);
	p.drawPoint(x,y);
    }
    else						// set for level-specific
    {
	if (!files.contains(level))
	{
	    qDebug() << "need new pixmap for level" << level;
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
    qDebug() << "level=" << level << " prep=" << preparedFor;

    if (preparedFor==level) return;			// already ready for level?

    QElapsedTimer timer;
    timer.start();

    sprites.clear();
    sprites.resize(Obj::num_sprites);
    greysprites.clear();
    greysprites.resize(Obj::num_sprites);
    brightsprites.clear();
    brightsprites.resize(Obj::num_sprites);

    QPixmap src;					// source pixmap, if any
    if (files.contains(level))				// specific one present
    {
	src = files[level];				// pixmap for this level
	qDebug() << "  prepare for level" << level;
    }
    else
    {
	qDebug() << "  prepare from raw/edited";
    }

    for (int y = 0; y<ynum; ++y)
    {
	for (int x = 0; x<xnum; ++x)
	{
	    int i = y*xnum + x;

	    QPixmap px;
	    if (!src.isNull()) px = src.copy(x*base_width, y*base_height, base_width, base_height);
	    else px = rawsprites[i];

            if (magnification!=Sprites::Normal)
            {
		    px = px.scaled(sprite_width, sprite_height,
				   Qt::IgnoreAspectRatio,
				   Qt::SmoothTransformation);
            }

            if (i==Obj::Blip || i==Obj::Blip2)
            {
                px.setMask(px.createHeuristicMask());
            }

            sprites[i] = px;

	    // In the original BBC version, which can be seen working in the
	    // walkthough video at https://www.youtube.com/watch?v=jfk-ka9ty8A,
	    // the screen flashing as time runs out is implemented by changing
	    // all black pixels to white.  That was easy on the BBC with its
	    // hardware palette, but not so easy with full RGB colour range.
	    //
	    // The bright white images for the screen flashing, and also the
	    // grey versions for use when paused, are therefore precomputed
	    // at this point.  These conversions may be relatively slow, but
	    // this initialisation is only done at most once per level.

	    QImage img1 = px.toImage();			// original pixmap for sprite
							// greyscale verison for pause
	    QImage img2 = img1.convertToFormat(QImage::Format_Grayscale8);
	    greysprites[i] = QPixmap::fromImage(img2);

#ifdef FLASH_INVERT_EVERYTHING
	    img2.invertPixels();			// inverted greyscale version
	    brightsprites[i] = QPixmap::fromImage(img2);
#else
	    for (int xp = 0; xp<img1.width(); ++xp)
	    {
		    for (int yp = 0; yp<img1.height(); ++yp)
		    {					// change all black pixels to white
			    QColor col = img1.pixelColor(xp, yp);
			    if (col==Qt::black) img1.setPixelColor(xp, yp, Qt::white);

		    }
	    }
	    brightsprites[i] = QPixmap::fromImage(img1);
#endif
	}
    }

    preparedFor = level;
    qDebug() << "  prepared for level" << preparedFor << "took" << timer.elapsed() << "ms";
}


bool Sprites::hasMultiLevels() const
{
    qDebug() << "c0" << files.contains(0) << "count" << files.count();
    return (files.count()>=2 || (!files.contains(0) && files.count()>=1));
}


void Sprites::removeMultiLevels()
{
    qDebug();
    files.clear();					// drastic, but what we need
    multiRemoved = true;				// note for save later
}


const QPixmap &Sprites::get(Obj::Type obj, Sprites::GetFlag flag) const
{
	if (flag==Sprites::GetRaw)
	{
		Q_ASSERT(!rawsprites.isEmpty());
		return (rawsprites.at(obj));
	}

	Q_ASSERT(!sprites.isEmpty());
	if (flag==Sprites::GetGrey) return (greysprites.at(obj));
	if (flag==Sprites::GetBright) return (brightsprites.at(obj));
	return (sprites.at(obj));
}
