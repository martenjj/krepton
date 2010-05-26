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

#include <kdebug.h>
#include <kstandarddirs.h>

#include <qpixmap.h>
#include <qbitmap.h>
#include <qpixmapcache.h>
#include <qmime.h>

#include "pixmaps.h"


//  Need to pass round and return pixmaps as value here (this is efficient),
//  not pointers to them.  See documentation for QPixmapCache::find(const QString &)
//  for the reason why.

const QPixmap getPixmap(const char *key)
{
	QPixmap pm;

	if (QPixmapCache::find(key,pm)) return (pm);

	if (!pm.load(KGlobal::dirs()->findResource("graphics",(QString(key)+".png"))))
        {
		kdDebug() << k_funcinfo << "cannot load pixmap '" << key << "'" << endl;
		pm.resize(16,16); pm.fill(Qt::red);
        }
	else pm.setMask(pm.createHeuristicMask());

        QPixmapCache::insert(key,pm);			// save in cache
        return (pm);
}


const QPixmap Pixmaps::find(Pixmaps::type p,bool setMimeSource)
{
	QString key;
	switch (p)
	{
case Key:	key = "key";		break;
case Crown:	key = "crown";		break;
case Back:	key = "back";		break;
case Pause:	key = "pause";		break;
case Started:	key = "started";	break;
case Playing:	key = "playing";	break;
case Finished:	key = "finished";	break;
case Unknown:	key = "unknown";	break;
case Password:	key = "password";	break;
case Unplayed:	key = "unplayed";	break;
default:	key = "unknown";	break;
	}

        QPixmap pm = getPixmap(key);
        if (setMimeSource)				// set for labels/tooltips
        {
            QMimeSourceFactory::defaultFactory()->setPixmap(QString("pixmap_")+key,pm);
        }
        return (pm);
}


const QPixmap Pixmaps::findLives(int l)
{
	if (l<0) l = 0;
	if (l>3) l = 3;

	const QString key = QString("lives%1").arg(l);
	return (getPixmap(key));
}
