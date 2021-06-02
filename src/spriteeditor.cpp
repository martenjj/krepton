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

#include "krepton.h"
#include "spriteeditor.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>

#include <dialogbase.h>

#include <kcolorbutton.h>

#include "spritepreview.h"
#include "spritegrid.h"
#include "sprites.h"
#include "objectlistbox.h"


SpriteEditor::SpriteEditor(QWidget *parent)
	: QWidget(parent)
{
	qDebug();

        setObjectName("SpriteEditor");
	setWindowTitle(i18n("Sprite Editor"));

	sprites = NULL;
	current_sprite = Obj::None;

	QGridLayout *gl = new QGridLayout(this);
	gl->setRowStretch(5,5);
	gl->setColumnStretch(8,5);
	gl->setColumnMinimumWidth(1,DialogBase::horizontalSpacing());
	gl->setColumnMinimumWidth(3,DialogBase::horizontalSpacing());
	gl->setColumnMinimumWidth(5,DialogBase::horizontalSpacing());
	gl->setRowMinimumHeight(2,DialogBase::verticalSpacing());
	gl->setRowMinimumHeight(5,DialogBase::verticalSpacing());
	gl->setRowMinimumHeight(6,2*DialogBase::verticalSpacing());

	sprite_list = new ObjectListBox(true,this);
	sprite_list->setToolTip(i18n("The list of available sprites to edit"));
	connect(sprite_list, SIGNAL(currentRowChanged(int)), SLOT(selectedSprite(int)));
	gl->addWidget(sprite_list,1,0,5,1);

	QLabel *label = new QLabel("Sprite &list:",this);
	label->setBuddy(sprite_list);
	gl->addWidget(label,0,0,Qt::AlignLeft);

	preview_sprite = new SpritePreview(this);
	preview_sprite->setToolTip(i18n("The sprite currently being edited"));
	gl->addWidget(preview_sprite,1,2,1,3,Qt::AlignLeft);

	label = new QLabel("Preview:", this);
	gl->addWidget(label,0,2,1,3,Qt::AlignLeft);

	left_color = new KColorButton(this);
	left_color->setFixedSize(40,30);
	left_color->setColor(Qt::white);
	left_color->setToolTip(i18n("The color placed by the left mouse button"));
	gl->addWidget(left_color,4,2,Qt::AlignLeft|Qt::AlignTop);

	label = new QLabel(i18n("Left:"), this);
	label->setBuddy(left_color);
	gl->addWidget(label,3,2,Qt::AlignLeft);

	right_color = new KColorButton(this);
	right_color->setFixedSize(40,30);
	right_color->setColor(Qt::black);
	right_color->setToolTip(i18n("The color placed by the right mouse button"));
	gl->addWidget(right_color,4,4,Qt::AlignLeft|Qt::AlignTop);

	label = new QLabel(i18n("Right:"), this);
	label->setBuddy(right_color);
	gl->addWidget(label,3,4,Qt::AlignLeft);

	grid_sprite = new SpriteGrid(this);
	gl->addWidget(grid_sprite,1,6,5,2,Qt::AlignLeft|Qt::AlignTop);

	label = new QLabel("S&prite:",this);
	label->setBuddy(grid_sprite);
	gl->addWidget(label,0,6,Qt::AlignLeft);
	connect(grid_sprite,SIGNAL(pressedButton(int,int,int)),
		this,SLOT(pressedButton(int,int,int)));
	connect(grid_sprite,SIGNAL(changedCoordinates(int,int)),
		SIGNAL(coordinatePosition(int,int)));

	adjustSize();

        sprite_list->setFocus();
	QTimer::singleShot(0, this, [this]() { sprite_list->setCurrentRow(static_cast<int>(current_sprite)); });
}


void SpriteEditor::setSprites(Sprites *ss)
{
	sprites = ss;
	updateSpriteDisplay();
}


void SpriteEditor::selectedSprite(int i)
{
	current_sprite = static_cast<Obj::Type>(i);
	updateSpriteDisplay();
}


void SpriteEditor::pressedButton(int button,int x,int y)
{
	QColor col;
	if (button & Qt::LeftButton) col = left_color->color();
	else if (button & Qt::RightButton) col = right_color->color();
	else return;

	Q_ASSERT(sprites!=NULL);
	sprites->setPixel(current_sprite,x,y,col);
	emit changedSprite();
}


void SpriteEditor::updateSpriteDisplay()
{
	Q_ASSERT(sprites!=NULL);
	preview_sprite->setSprite(sprites, current_sprite);
	grid_sprite->setSprite(sprites, current_sprite);
}
