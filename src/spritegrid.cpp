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

#include <qpainter.h>
#include <qimage.h>
#include <qevent.h>

#include "krepton.h"
#include "sprites.h"

#include "spritegrid.h"


static const int previewscale = 8;


SpriteGrid::SpriteGrid(QWidget *parent) : QFrame(parent)
{
	sprites = NULL;
	object = Obj::Empty;

	setFrameStyle(QFrame::Box|QFrame::Raised);
	setLineWidth(2);
	setMidLineWidth(1);
	border = lineWidth() * 2 + midLineWidth();
	setFixedSize(Sprites::base_width*previewscale+border*2-1,
	       Sprites::base_height*previewscale+border*2-1);
	setMouseTracking(true);
}


void SpriteGrid::paintEvent(QPaintEvent *ev)
{
	if (sprites==NULL || object>=Obj::num_sprites) return;
	QImage img = sprites->getRaw(object).toImage();

	QPainter p(this);
	for (int y = 0; y<Sprites::base_height; ++y)
	{
		for (int x = 0; x<Sprites::base_width; ++x)
		{
			QColor c = img.pixel(x,y);
			p.fillRect(border+x*previewscale,border+y*previewscale,
				   (previewscale-1),(previewscale-1),c);
		}
	}
}


void SpriteGrid::mousePressEvent(QMouseEvent *e)
{
	if (e->button()==Qt::LeftButton || e->button()==Qt::RightButton)
	{
		if (e->x()<border || e->x()>(width()-border) ||
		    e->y()<border || e->y()>(height()-border)) return;

		int x = (e->x()-border)/previewscale;
		int y = (e->y()-border)/previewscale;
		emit pressedButton(e->button(),x,y);
	}
}


void SpriteGrid::mouseMoveEvent(QMouseEvent *e)
{
	if (e->x()<border || e->x()>(width()-border) ||
	    e->y()<border || e->y()>(height()-border)) return;

	int x = (e->x()-border)/previewscale;
	int y = (e->y()-border)/previewscale;

	int b = e->buttons() & (Qt::LeftButton|Qt::RightButton);
	if (b!=0) emit pressedButton(b,x,y);
	emit changedCoordinates(x, y);
}
