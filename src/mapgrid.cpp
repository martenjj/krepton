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

#include "mapgrid.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>
#include <qevent.h>
#include <qscrollbar.h>

#include "krepton.h"
#include "sprites.h"
#include "mapedit.h"


MapGrid::MapGrid(QWidget *parent)
	: QScrollArea(parent)
{
	qDebug();

	// TODO: instead of a separate widget, make this a QAbstractScrollArea
	// and handle paint events on the viewport instead.

        mWidget = new MapGridWidget(this);
	mWidget->setMouseTracking(true);
	setMouseTracking(true);
        setWidget(mWidget);

	connect(mWidget,SIGNAL(pressedButton(int,int,int)),SIGNAL(pressedButton(int,int,int)));
	connect(mWidget,SIGNAL(changedCoordinates(int,int)),SIGNAL(changedCoordinates(int,int)));

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	horizontalScrollBar()->setSingleStep(Sprites::base_width);
	verticalScrollBar()->setSingleStep(Sprites::base_width);

	qDebug() << "done";
}


MapGridWidget::MapGridWidget(QWidget *parent)
	: QWidget(parent)
{
	qDebug();

	sprites = NULL;
	map = NULL;
	showtrans = false;
	showsel = false;
	xtrans = ytrans = 0;
}


void MapGrid::setMap(MapEdit *mm)
{
	mWidget->setMap(mm);
}


void MapGridWidget::setMap(MapEdit *mm)
{
	map = mm;
        update();

	if (map==NULL)
	{
		setEnabled(false);
		return;
	}

	const int cwidth = map->getWidth()*Sprites::base_width;
	const int cheight = map->getHeight()*Sprites::base_height;
	resize(cwidth,cheight);
	setMaximumSize(cwidth,cheight);
	setEnabled(true);
}


QVector<QPoint> MapGridWidget::previewBlipRoute(int x, int y)
{
	qDebug() << "blip at" << x << y;

	const int startx = x;
	const int starty = y;
	QVector<QPoint> route;

	Monster m(x, y, Obj::Blip);
	map->blipInitialDirection(&m);

	// It is not possible to find the end of the spirit route simply by
	// its returning to its starting point, as it may simply be passing it
	// in another direction.  It is also not possible to detect returning
	// to the starting point moving in the same direction, as the initial
	// direction from blipInitialDirection() may have been wrong.
	// Therefore the position and direction of the spirit is noted when it
	// has moved one square, when the direction of travel should be
	// correct.  When the spirit returns to the same position travelling
	// in the same direction, that is definitely the end of its route.
	int returnx = -1;
	int returny = -1;
	Orientation::Type returnorient;

	while (true)					// until end of route
	{
		if (map->updateBlip(&m))
		{
			int mx = m.xpos;
			int my = m.ypos;
			qDebug() << "  moved to" << mx << my;
			route.append(QPoint(mx*Sprites::base_width+(Sprites::base_width/2), my*Sprites::base_height+(Sprites::base_height/2)));

			if (mx==returnx && my==returny && m.orientation==returnorient)
			{
				qDebug() << "  returned";
				break;
			}

			if (returnx==-1)
			{
				returnx = mx;
				returny = my;
				returnorient = m.orientation;
				qDebug() << "  return to" << returnx << returny
					 << "orient" << map->showDirection(returnorient);
			}

			Obj::Type obj = map->getCell(mx, my);
			if (obj==Obj::Cage)
			{
				qDebug() << "  caught";
				break;
			}
		}
		else
		{
			qDebug() << "  cannot move";
			break;
		}

		// The last screen of episode "Now" has a spirit run
		// 362 squares long.
		if (route.count()>400)
		{
			qDebug() << "  too long";
			break;
		}
	}

	qDebug() << "route found with" << route.count() << "points";
	return (route);
}





void MapGridWidget::paintEvent(QPaintEvent *ev)
{
	QPainter p(this);

	if (map==NULL || sprites==NULL)			// no contents to draw
	{						// but we must do this
		p.eraseRect(p.window());		// because of WRepaintNoErase
		return;
	}

	const int mapwidth = map->getWidth();
	const int mapheight = map->getHeight();

	for (int y = 0; y<mapheight; ++y)
	{
		int aty = y*Sprites::base_height;
		//if (aty>(clipy+cliph)) break;
		//if ((aty+Sprites::base_height)<clipy) continue;

		for (int x = 0; x<mapwidth; ++x)
		{
			int atx = x*Sprites::base_width;
			//if (atx>(clipx+clipw)) break;
			//if ((atx+Sprites::base_width)<clipx) continue;

			p.drawPixmap(atx,aty,sprites->getRaw(map->getCell(x,y)));
		}
	}

	if (showtrans)
	{
		const TransporterList tl = map->getTransportersList();
		for (int i = 0; i<tl.count(); ++i)
		{
			int ox,oy,dx,dy;
			map->transporterGet(i,&ox,&oy,&dx,&dy);
			ox = (ox*Sprites::base_width)-(Sprites::base_width/2);
			oy = (oy*Sprites::base_height)-(Sprites::base_height/2);
			dx = (dx*Sprites::base_width)-(Sprites::base_width/2);
			dy = (dy*Sprites::base_height)-(Sprites::base_height/2);

			QPen pen(Qt::white,3);
			p.setPen(pen);
			p.drawLine(ox,oy,dx,dy);

			pen.setWidth(0);
			p.setPen(pen);
			p.setBrush(Qt::white);
			p.drawEllipse(dx-4,dy-4,9,9);
		}
	}

	// TODO: GUI option to show these
	if (true)
	{
		for (int y = 0; y<mapheight; ++y)
		{
			for (int x = 0; x<mapwidth; ++x)
			{
				Obj::Type obj = map->getCell(x, y);
				if (obj!=Obj::Blip) continue;

				QVector<QPoint> route = previewBlipRoute(x, y);
				if (!route.isEmpty())
				{
					route.prepend(QPoint(x*Sprites::base_width+(Sprites::base_width/2), y*Sprites::base_height+(Sprites::base_height/2)));
					QPen pen(Qt::white, 2);
					p.setPen(pen);
					p.drawPolyline(route.data(), route.count());
				}
			}
		}
	}

	if (showsel && xtrans>0 && ytrans>0)
	{
		int tx = (xtrans*Sprites::base_width)-(Sprites::base_width/2);
		int ty = (ytrans*Sprites::base_height)-(Sprites::base_height/2);

		QPen pen(Qt::white,3);
		p.setPen(pen);
		p.drawLine(tx-9,ty-9,tx+9,ty+9);
		p.drawLine(tx-9,ty+9,tx+9,ty-9);
	}
}


void MapGridWidget::mousePressEvent(QMouseEvent *ev)
{
	if (!(ev->button()==Qt::LeftButton || ev->button()==Qt::RightButton)) return;

	int x = ev->x()/Sprites::base_width;
	int y = ev->y()/Sprites::base_height;
	emit pressedButton(ev->button(),x,y);
}


void MapGridWidget::mouseMoveEvent(QMouseEvent *ev)
{
	int x = ev->x()/Sprites::base_width;
	int y = ev->y()/Sprites::base_height;

	int b = ev->buttons() & (Qt::LeftButton|Qt::RightButton);
	if (b!=0) emit pressedButton(b,x,y);
	emit changedCoordinates(x,y);
}


void MapGridWidget::leaveEvent(QEvent *ev)
{
	emit changedCoordinates(-1,-1);
}


void MapGrid::updatedCell(int x,int y)
{
	// TODO: if spirit routes are being shown, and the changed cell could
	// potentially affect a spirit route (replacing a traverseable object
	// with a non traverseable one) then the entire map needs to be
	// repainted.

	mWidget->repaint(x*Sprites::base_width,y*Sprites::base_height,
	                 Sprites::base_width,Sprites::base_height);
}


void MapGrid::showTransporters(bool state)
{
	mWidget->showTransporters(state);
}


void MapGridWidget::showTransporters(bool state)
{
	showtrans = state;
}


void MapGrid::showSelectedTransporter(bool state)
{
	mWidget->showSelectedTransporter(state);
}


void MapGridWidget::showSelectedTransporter(bool state)
{
	showsel = state;
}


void MapGrid::selectedTransporter(int item)
{
	mWidget->selectedTransporter(item);
}


void MapGridWidget::selectedTransporter(int item)
{
	qDebug() << "item=" << item;

	int ox = 0;
	int oy = 0;

	if (item>=0) map->transporterGet(item,&ox,&oy);
	xtrans = ox;
	ytrans = oy;
}


void MapGrid::setSprites(Sprites *ss)
{
	mWidget->setSprites(ss);
}


void MapGridWidget::setSprites(Sprites *ss)
{
	sprites = ss;
}


void MapGrid::update()
{
	mWidget->update();
}
