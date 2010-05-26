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

#include <qlabel.h>
#include <qpixmap.h>

#include "krepton.h"
#include "sprites.h"

#include "spritepreview.h"


SpritePreview::SpritePreview(QWidget *parent,const char *name) : QLabel(parent,name)
{
	sprites = NULL;
	object = Obj::Empty;

	setFrameStyle(QFrame::Box|QFrame::Sunken);
	setLineWidth(1);
	setMidLineWidth(2);
	int border = lineWidth()*2+midLineWidth();
	setFixedSize(Sprites::base_width+border*2,Sprites::base_height+border*2);
}


void SpritePreview::setSprite(Sprites *ss,Obj::Type tt)
{
	sprites = ss;
	object = tt;

	if (object==((Obj::Type) -1)) setEnabled(false);
	else
	{
		setEnabled(true);
		setPixmap(sprites->getRaw(object));
	}
}
