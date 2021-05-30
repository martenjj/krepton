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
#include <qevent.h>
#include <qscrollbar.h>

#include "krepton.h"
#include "sprites.h"
#include "mapedit.h"


MapGrid::MapGrid(QWidget *parent)
	: QScrollArea(parent)
{
	qDebug();

	QWidget *w = new QWidget(this);
	w->setMouseTracking(true);
	w->setAttribute(Qt::WA_OpaquePaintEvent);
	w->setAttribute(Qt::WA_NoSystemBackground);
	w->installEventFilter(this);
	setWidget(w);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	horizontalScrollBar()->setSingleStep(Sprites::base_width);
	verticalScrollBar()->setSingleStep(Sprites::base_width);

	sprites = NULL;
	map = NULL;
	showtrans = false;
	showsel = false;
	showspiritroutes = false;
	xtrans = ytrans = 0;
}


void MapGrid::setMap(MapEdit *mm)
{
	map = mm;
        update();

	if (map==NULL)					// removing the map
	{
		widget()->setEnabled(false);
		return;
	}

	const int cwidth = map->getWidth()*Sprites::base_width;
	const int cheight = map->getHeight()*Sprites::base_height;
	widget()->setFixedSize(cwidth, cheight);
	widget()->setEnabled(true);
}


// Work out a spirit route, given its starting position and the current
// state of the map, as a list of squares traversed.  It starts at the
// spirit position and ends either when it encounters a cage, or returns
// to the starting point travelling in the same direction.  The route is
// drawn on the map by plotBlipRoute() below.

static QVector<QPoint> previewBlipRoute(int x, int y, MapEdit *map)
{
	qDebug() << "blip at" << x+1 << y+1;

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
	Orientation::Type returnorient = Orientation::None;

	// This limit is to make sure that the route calculation does not go
	// on for ever in the event that the spirit does not return to its
	// starting point.  It is probably not possible, for a static map,
	// to construct a situation where a spirit does not either get caught
	// in a cage or return to its starting point, but this is an emergency
	// stop just in case.
	//
	// Experimenting with small maps would suggest that the maximum length
	// of a spirit route which can be achieved is about 70% of the total
	// map area.  This limit is not likely to be reached by real maps which
	// are actually interesting to play:  the longest route observed in the
	// Repton 3 canon is 362 squares long in "Now" screen 8 (53%).
	//
	// So the limit is set at 60% of the total map area.  If there is a map
	// that ever hits this limit, I'd be interested to see it...
	//
	// The limit can be hit if the spirit is in free space or is confused
	// at its starting position, though, because it may never return to
	// the next square after its start (the one where returnx/returny are
	// noted).  So the limit does need to be implemented.
	const int limit = (map->getWidth()*map->getHeight()*100)/60;

	while (true)					// until end of route
	{
		if (map->updateBlip(&m))
		{
			int mx = m.xpos;
			int my = m.ypos;
			qDebug() << "  moved to" << mx+1 << my+1;
			route.append(QPoint(mx, my));

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
				qDebug() << "  return to" << returnx+1 << returny+1
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

		// Check that the route length has not reached the limit
		// as calculated above.
		if (route.count()>limit)
		{
			qDebug() << "  reached limit" << limit;
			break;
		}
	}

	qDebug() << "route found with" << route.count() << "points";
	return (route);
}


// Positions within a square, for the blip route plotting offset.

#define CENTRE		0
#define TOPLEFT		1
#define TOPRIGHT	2
#define BOTTOMLEFT	3
#define BOTTOMRIGHT	4


static QPoint cellPosition(int x, int y, int within)
{
	QPoint p(x*Sprites::base_width+(Sprites::base_width/2),
		 y*Sprites::base_height+(Sprites::base_height/2));

	const int offsetx = Sprites::base_width/4;
	const int offsety = Sprites::base_height/4;
	if (within==TOPLEFT)
	{
		p.rx() -= offsetx;
		p.ry() -= offsety;
	}
	else if (within==TOPRIGHT)
	{
		p.rx() += offsetx;
		p.ry() -= offsety;
	}
	else if (within==BOTTOMRIGHT)
	{
		p.rx() += offsetx;
		p.ry() += offsety;
	}
	else if (within==BOTTOMLEFT)
	{
		p.rx() -= offsetx;
		p.ry() += offsety;
	}

	return (p);
}


static Orientation::Type movementDirection(const QPoint &prevpnt, const QPoint &thispnt)
{
	Orientation::Type dir = Orientation::None;
	if (prevpnt.x()==thispnt.x())			// vertical movement
	{
		if (thispnt.y()>prevpnt.y()) dir = Orientation::South;
		else dir = Orientation::North;
	}
	else if (prevpnt.y()==thispnt.y())		// horizontal movement
	{
		if (thispnt.x()>prevpnt.x()) dir = Orientation::East;
		else dir = Orientation::West;
	}

	if (dir==Orientation::None) qWarning() << "cannot find direction from" << prevpnt << "to" << thispnt;
	return (dir);
}


// Given a spirit route as calcuated by previewBlipRoute() above, generate
// a polyline to draw on the map.  Simply drawing along the centre line of
// the route is simple but does not really show what is happening if the
// spirit turns round or returns along the same route.  The drawn route is
// therefore offset to correspond with the spirit logic of always following
// the wall to its left.

static QVector<QPoint> plotBlipRoute(const QVector<QPoint> &route)
{
	QVector<QPoint> plot;
	plot.reserve(route.size()*2);

#ifdef SPIRIT_ROUTE_CENTRE
	for (const QPoint &pp : route)
	{
		plot.append(cellPosition(pp.x(), pp.y(), CENTRE));
	}
#else // SPIRIT_ROUTE_CENTRE
	for (int i = 1; i<route.count()-1; ++i)
	{
		const QPoint &pnt = route[i];

		// Find the orientation that the spirit is currently travelling in.
		// This means the direction from the previous point to this one;
		// because of starting at 1 above there will always be a previous point.
		const QPoint &prevpnt = route[i-1];
		Orientation::Type thisdir = movementDirection(prevpnt, pnt);
		if (thisdir==Orientation::None) continue;

		// Then find the orientation that the spirit will leave this
		// point travelling in.  This means the direction from this
		// point to the next one; because of the "-1" above there will
		// always be a next point.
		const QPoint &nextpnt = route[i+1];
		Orientation::Type nextdir = movementDirection(pnt, nextpnt);
		if (nextdir==Orientation::None) continue;

		// For the first point (i.e. one step on from the spirit's starting
		// position), calculate the starting position corresponding to this
		// step orientation.
		if (i==1)
		{
			int startpoint = CENTRE;
			if (thisdir==Orientation::North) startpoint = BOTTOMLEFT;
			else if (thisdir==Orientation::East) startpoint = TOPLEFT;
			else if (thisdir==Orientation::South) startpoint = TOPRIGHT;
			else if (thisdir==Orientation::West) startpoint = BOTTOMRIGHT;

			// If the spirit will not be returning to this point at the end
			// if its route, then it needs the step from its start point
			// to here to be drawn first.
			if (pnt!=route.last()) plot.append(cellPosition(prevpnt.x(), prevpnt.y(), startpoint));

			// Then the same position within this point.
			plot.append(cellPosition(pnt.x(), pnt.y(), startpoint));
		}

		// Find the arrival point in the next square corresponding to that
		// movement direction.
		int endpoint = CENTRE;
		if (nextdir==Orientation::North) endpoint = BOTTOMLEFT;
		else if (nextdir==Orientation::East) endpoint = TOPLEFT;
		else if (nextdir==Orientation::South) endpoint = TOPRIGHT;
		else if (nextdir==Orientation::West) endpoint = BOTTOMRIGHT;

		// For a right turn, the spirit needs first to move to the far
		// side of the current square in the current direction before
		// turning to go on to the next square.
		if (thisdir==Orientation::North && nextdir==Orientation::East)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), TOPLEFT));
		}
		else if (thisdir==Orientation::East && nextdir==Orientation::South)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), TOPRIGHT));
		}
		else if (thisdir==Orientation::South && nextdir==Orientation::West)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), BOTTOMRIGHT));
		}
		else if (thisdir==Orientation::West && nextdir==Orientation::North)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), BOTTOMLEFT));
		}

		// But if the spirit is turning completely around, then it needs to
		// go around the far side of the current square first.
		if (thisdir==Orientation::North && nextdir==Orientation::South)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), TOPLEFT));
			plot.append(cellPosition(pnt.x(), pnt.y(), TOPRIGHT));
		}
		else if (thisdir==Orientation::East && nextdir==Orientation::West)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), TOPRIGHT));
			plot.append(cellPosition(pnt.x(), pnt.y(), BOTTOMRIGHT));
		}
		else if (thisdir==Orientation::South && nextdir==Orientation::North)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), BOTTOMRIGHT));
			plot.append(cellPosition(pnt.x(), pnt.y(), BOTTOMLEFT));
		}
		else if (thisdir==Orientation::West && nextdir==Orientation::East)
		{
			plot.append(cellPosition(pnt.x(), pnt.y(), BOTTOMLEFT));
			plot.append(cellPosition(pnt.x(), pnt.y(), TOPLEFT));
		}

		// Finally the spirit moves on to the destination point in
		// the next square.
		plot.append(cellPosition(nextpnt.x(), nextpnt.y(), endpoint));
	}
#endif // SPIRIT_ROUTE_CENTRE

	return (plot);
}


bool MapGrid::widgetPaintEvent(QPaintEvent *ev)
{
	QPainter p(widget());

	if (map==NULL || sprites==NULL)			 // no contents to draw
	{						 // but we must do this
		p.eraseRect(p.window());		 // because of WA_OpaquePaintEvent
		return (false);
	}

	const int mapwidth = map->getWidth();
	const int mapheight = map->getHeight();

	for (int y = 0; y<mapheight; ++y)
	{
		const int aty = y*Sprites::base_height;
		for (int x = 0; x<mapwidth; ++x)
		{
			const int atx = x*Sprites::base_width;
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

	if (showspiritroutes)
	{
		for (int y = 0; y<mapheight; ++y)
		{
			for (int x = 0; x<mapwidth; ++x)
			{
				Obj::Type obj = map->getCell(x, y);
				if (obj!=Obj::Blip) continue;

				QVector<QPoint> route = previewBlipRoute(x, y, map);
				if (route.isEmpty()) continue;

				route.prepend(QPoint(x, y));
				const QVector<QPoint> &plot = plotBlipRoute(route);

				QPen pen(Qt::white, 2);
				p.setPen(pen);
				p.drawPolyline(plot.data(), plot.count());
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

	return (false);			// propagate event

}


bool MapGrid::widgetMouseButtonPressEvent(QMouseEvent *ev)
{
	QMouseEvent *mev = static_cast<QMouseEvent *>(ev);
	if (mev->button()==Qt::LeftButton || mev->button()==Qt::RightButton)
	{
		int x = mev->x()/Sprites::base_width;
		int y = mev->y()/Sprites::base_height;
		emit pressedButton(mev->button(),x,y);
	}

	return (true);			// handled event
}


bool MapGrid::widgetMouseMoveEvent(QMouseEvent *ev)
{
	QMouseEvent *mev = static_cast<QMouseEvent *>(ev);

	int x = mev->x()/Sprites::base_width;
	int y = mev->y()/Sprites::base_height;

	int b = mev->buttons() & (Qt::LeftButton|Qt::RightButton);
	if (b!=0) emit pressedButton(b,x,y);
	emit changedCoordinates(x,y);

	return (true);					// handled event
}


bool MapGrid::widgetLeaveEvent(QEvent *ev)
{
	emit changedCoordinates(-1, -1);
	return (true);					// handled event
}


bool MapGrid::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj==widget())
	{
		switch (ev->type())
		{
case QEvent::Paint:
			return (widgetPaintEvent(static_cast<QPaintEvent *>(ev)));

case QEvent::MouseButtonPress:
			return (widgetMouseButtonPressEvent(static_cast<QMouseEvent *>(ev)));

case QEvent::MouseMove:
			return (widgetMouseMoveEvent(static_cast<QMouseEvent *>(ev)));

case QEvent::Leave:
			return (widgetLeaveEvent(ev));

default:		break;
		}
	}

	return (QScrollArea::eventFilter(obj, ev));
}


void MapGrid::updatedCell(int x,int y)
{
	// If spirit routes are being shown, any changed cell could
	// potentially affect a spirit route by replacing a traverseable
	// object with a non-traverseable one.  Therefore the entire map
	// needs to be repainted.  Otherwise, only the changed cell needs
	// to be repainted.
	if (showspiritroutes) widget()->update();
	else widget()->update(x*Sprites::base_width, y*Sprites::base_height,
			      Sprites::base_width, Sprites::base_height);
}


void MapGrid::showTransporters(bool state)
{
	showtrans = state;
        update();
}


void MapGrid::showSpiritRoutes(bool state)
{
	showspiritroutes = state;
        update();
}


void MapGrid::showSelectedTransporter(bool state)
{
	showsel = state;
        update();
}


void MapGrid::selectedTransporter(int item)
{
	qDebug() << "item=" << item;

	int ox = 0;
	int oy = 0;

	if (item>=0) map->transporterGet(item,&ox,&oy);
	xtrans = ox;
	ytrans = oy;

        update();
}


void MapGrid::setSprites(Sprites *ss)
{
	sprites = ss;
}


void MapGrid::update()
{
	widget()->update();
}
