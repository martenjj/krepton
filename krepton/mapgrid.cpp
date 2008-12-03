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

#include <qscrollview.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qptrlist.h>
#include <qpen.h>
#include <qcolor.h>

#include "krepton.h"
#include "sprites.h"
#include "map2.h"

#include "mapgrid.h"


MapGrid::MapGrid(QWidget *parent,const char *name)
	: QScrollView(parent,name,Qt::WResizeNoErase|Qt::WRepaintNoErase)
{
	kdDebug(0) << k_funcinfo << endl;

	sprites = NULL;
	map = NULL;
	object = Obj::Empty;
	showtrans = false;
	showsel = false;
	xtrans = ytrans = 0;

	setHScrollBarMode(QScrollView::AlwaysOn);
	setVScrollBarMode(QScrollView::AlwaysOn);
	horizontalScrollBar()->setLineStep(Sprites::base_width);
	verticalScrollBar()->setLineStep(Sprites::base_width);

	setResizePolicy(QScrollView::Manual);
	viewport()->setMouseTracking(true);

	kdDebug(0) << k_funcinfo << "done" << endl;
}


void MapGrid::setMap(MapEdit *mm)
{
	map = mm;
	updateContents();

	if (map==NULL)
	{
		setEnabled(false);
		return;
	}

	const int cwidth = map->getWidth()*Sprites::base_width;
	const int cheight = map->getHeight()*Sprites::base_height;
	resizeContents(cwidth,cheight);
	viewport()->setMaximumSize(cwidth,cheight);
	setEnabled(true);
}


void MapGrid::drawContents(QPainter *p,int clipx,int clipy,int clipw,int cliph)
{
	if (map==NULL || sprites==NULL)			// no contents to draw
	{						// but we must do this
		p->eraseRect(p->window());		// because of WRepaintNoErase
		return;
	}

//	kdDebug(0) << k_funcinfo << "showtrans=" << showtrans << endl;

	const int mapwidth = map->getWidth();
	const int mapheight = map->getHeight();

	for (int y = 0; y<mapheight; ++y)
	{
		int aty = y*Sprites::base_height;
		if (aty>(clipy+cliph)) break;
		if ((aty+Sprites::base_height)<clipy) continue;

		for (int x = 0; x<mapwidth; ++x)
		{
			int atx = x*Sprites::base_width;
			if (atx>(clipx+clipw)) break;
			if ((atx+Sprites::base_width)<clipx) continue;

			p->drawPixmap(atx,aty,sprites->getRaw(map->getCell(x,y)));
		}
	}

	if (showtrans)
	{
		const QPtrList<Transporter> tl = map->getTransportersList();
		for (unsigned int i = 0; i<tl.count(); ++i)
		{
			int ox,oy,dx,dy;
			map->transporterGet(i,&ox,&oy,&dx,&dy);
//			kdDebug(0) << k_funcinfo << "oxy=" << ox << "," << oy << " dxy=" << dx << "," << dy << endl;
			ox = (ox*Sprites::base_width)-(Sprites::base_width/2);
			oy = (oy*Sprites::base_height)-(Sprites::base_height/2);
			dx = (dx*Sprites::base_width)-(Sprites::base_width/2);
			dy = (dy*Sprites::base_height)-(Sprites::base_height/2);

			QPen pen(Qt::white,3);
			p->setPen(pen);
			p->drawLine(ox,oy,dx,dy);

			pen.setWidth(0);
			p->setPen(pen);
			p->setBrush(Qt::white);
			p->drawEllipse(dx-4,dy-4,9,9);
		}
	}

	if (showsel && xtrans>0 && ytrans>0)
	{
		kdDebug(0) << k_funcinfo << "xy=" << xtrans << "," << ytrans << endl;

		int tx = (xtrans*Sprites::base_width)-(Sprites::base_width/2);
		int ty = (ytrans*Sprites::base_height)-(Sprites::base_height/2);

		QPen pen(Qt::white,3);
		p->setPen(pen);
		p->drawLine(tx-9,ty-9,tx+9,ty+9);
		p->drawLine(tx-9,ty+9,tx+9,ty-9);
	}
}


void MapGrid::contentsMousePressEvent(QMouseEvent *e)
{
	if (!(e->button()==LeftButton || e->button()==RightButton)) return;

	int x = e->x()/Sprites::base_width;
	int y = e->y()/Sprites::base_height;
	emit pressedButton(e->button(),x,y);
}


void MapGrid::contentsMouseMoveEvent(QMouseEvent *e)
{
	int x = e->x()/Sprites::base_width;
	int y = e->y()/Sprites::base_height;

	int b = e->state() & (LeftButton|RightButton);
	if (b!=0) emit pressedButton(b,x,y);
	emit changedCoordinates(x,y);
}


void MapGrid::updatedCell(int x,int y)
{
	repaintContents(x*Sprites::base_width,y*Sprites::base_height,
			Sprites::base_width,Sprites::base_height,false);
}


void MapGrid::showTransporters(bool state)
{
//	kdDebug(0) << k_funcinfo << "state=" << state << endl;
	showtrans = state;
}

void MapGrid::showSelectedTransporter(bool state)
{
//	kdDebug(0) << k_funcinfo << "state=" << state << endl;
	showsel = state;
}

void MapGrid::selectedTransporter(int item)
{
	kdDebug(0) << k_funcinfo << "item=" << item << endl;

	int ox = 0;
	int oy = 0;

	if (item>=0) map->transporterGet(item,&ox,&oy);
	xtrans = ox;
	ytrans = oy;
}