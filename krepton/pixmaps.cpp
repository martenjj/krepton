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

#include <kstandarddirs.h>

#include <qpixmap.h>
#include <qbitmap.h>

#include "pixmaps.h"


const QPixmap *Pixmaps::key = NULL;
const QPixmap *Pixmaps::crown = NULL;
const QPixmap *Pixmaps::back = NULL;
const QPixmap *Pixmaps::pause = NULL;

const QPixmap *Pixmaps::lives[4] = { NULL, NULL, NULL, NULL };

void loadPixmap(QPixmap **pix,const char *name)
{
	QPixmap *p = *pix;
	if (p!=NULL) return;

	p = new QPixmap(KGlobal::dirs()->findResource("graphics",name));
	p->setMask(p->createHeuristicMask());
	*pix = p;
}



const QPixmap *Pixmaps::find(Pixmaps::type p)
{
	switch (p)
	{
case Key:	loadPixmap((QPixmap **) &key,"key.png");
		return (key);

case Crown:	loadPixmap((QPixmap **) &crown,"crown.png");
		return (crown);

case Back:	loadPixmap((QPixmap **) &back,"back.png");
		return (back);

case Pause:	loadPixmap((QPixmap **) &pause,"pause.png");
		return (pause);

default:	return (NULL);
	}
}


const QPixmap *Pixmaps::findLives(int l)
{
	if (l<0) l = 0;
	if (l>3) l = 3;

	const QString file = QString("lives%1.png").arg(l);
	loadPixmap((QPixmap **) &lives[l],file);
	return (lives[l]);
}
