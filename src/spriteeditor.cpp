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

#include <qwidget.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QCloseEvent>

#include <kcolorbtn.h>
#include <kdialog.h>
#ifdef EDITOR_3_WINDOWS
#include <kpushbutton.h>
#endif

#include "krepton.h"
#include "spritepreview.h"
#include "spritegrid.h"
#include "sprites.h"
#include "objectlistbox.h"
#include "coordlabel.h"

#include "spriteeditor.h"


SpriteEditor::SpriteEditor(QWidget *parent,Sprites **ss)
	: QWidget(parent,QString::null,Qt::WType_TopLevel)
{
	kdDebug(0) << k_funcinfo << endl;

	setCaption("Sprite Editor");
//	setFixedSize(525, 305);

	sprites = ss;
	current_sprite = Obj::Empty;

	Q3GridLayout *gl = new Q3GridLayout(this,8,9,KDialog::marginHint(),KDialog::spacingHint());
	gl->setRowStretch(5,5);
	gl->setColStretch(8,5);
	gl->addColSpacing(1,KDialog::spacingHint());
	gl->addColSpacing(3,KDialog::spacingHint());
	gl->addColSpacing(5,KDialog::spacingHint());
	gl->addRowSpacing(2,KDialog::spacingHint());
	gl->addRowSpacing(5,KDialog::spacingHint());
	gl->addRowSpacing(6,KDialog::marginHint());

	sprite_list = new ObjectListBox(true,this);
	QToolTip::add(sprite_list, "The list of available sprites to edit");
	connect(sprite_list, SIGNAL(highlighted(int)),
		this, SLOT(selectedSprite(int)));
	gl->addMultiCellWidget(sprite_list,1,5,0,0);

	QLabel *label = new QLabel("Sprite &list:",this);
	label->setBuddy(sprite_list);
	gl->addWidget(label,0,0,Qt::AlignLeft);

	preview_sprite = new SpritePreview(this);
	QToolTip::add(preview_sprite, "The sprite currently being edited");
	gl->addMultiCellWidget(preview_sprite,1,1,2,4,Qt::AlignLeft);

	label = new QLabel("Preview:", this);
	gl->addMultiCellWidget(label,0,0,2,4,Qt::AlignLeft);

	left_color = new KColorButton(this);
	left_color->setFixedSize(40,30);
	left_color->setColor(Qt::white);
	QToolTip::add(left_color, "The color placed by the left mouse button");
	gl->addWidget(left_color,4,2,Qt::AlignLeft|Qt::AlignTop);

	label = new QLabel(left_color, "L&eft:", this);
	label->setBuddy(left_color);
	gl->addWidget(label,3,2,Qt::AlignLeft);

	right_color = new KColorButton(this);
	right_color->setFixedSize(40,30);
	right_color->setColor(Qt::black);
	QToolTip::add(right_color, "The color placed by the right mouse button");
	gl->addWidget(right_color,4,4,Qt::AlignLeft|Qt::AlignTop);

	label = new QLabel(right_color, "&Right:", this);
	label->setBuddy(right_color);
	gl->addWidget(label,3,4,Qt::AlignLeft);

	grid_sprite = new SpriteGrid(this);
	gl->addMultiCellWidget(grid_sprite,1,5,6,7,Qt::AlignLeft|Qt::AlignTop);

	label = new QLabel("S&prite:",this);
	label->setBuddy(grid_sprite);
	gl->addWidget(label,0,6,Qt::AlignLeft);
	connect(grid_sprite,SIGNAL(pressedButton(int,int,int)),
		this,SLOT(pressedButton(int,int,int)));
	connect(grid_sprite,SIGNAL(changedCoordinates(int,int)),
		this,SLOT(updateCoordinates(int,int)));

#ifdef EDITOR_3_WINDOWS
	KPushButton *close_button = new KPushButton("&Close", this);
	close_button->setDefault(true);
	QToolTip::add(close_button, "Close this window");
	gl->addWidget(close_button,7,0,Qt::AlignLeft);
	connect(close_button,SIGNAL(clicked()),this,SLOT(close()));
#endif

	coord = new CoordLabel(this);
	gl->addWidget(coord,0,7,Qt::AlignRight);

	adjustSize();

        sprite_list->setFocus();
}


void SpriteEditor::selectedSprite(int i)
{
	current_sprite = (Obj::Type) i;
	updateChilds();
}


void SpriteEditor::pressedButton(int button,int x,int y)
{
	QColor col;
	if (button & Qt::LeftButton) col = left_color->color();
	else if (button & Qt::RightButton) col = right_color->color();
	else return;

	(*sprites)->setPixel(current_sprite,x,y,col);
	emit changedSprite();
}


void SpriteEditor::updateCoordinates(int x,int y)
{
	coord->setXY(x,y);
}


void SpriteEditor::updateChilds()
{
	preview_sprite->setSprite(*sprites,current_sprite);
	grid_sprite->setSprite(*sprites,current_sprite);
	grid_sprite->repaint(false);	
}


void SpriteEditor::closeEvent(QCloseEvent *e)
{
	kdDebug(0) << k_funcinfo << endl;

	QWidget::closeEvent(e);
	hide();
	emit closed();
}
