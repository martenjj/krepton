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

#include <qlabel.h>
#include <q3scrollview.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <qgridlayout.h>
#include <QCloseEvent>

#include <kdialog.h>
#ifdef EDITOR_3_WINDOWS
#include <kpushbutton.h>
#endif

#include "krepton.h"
#include "sprites.h"
#include "map2.h"
#include "objectlistbox.h"
#include "spritepreview.h"
#include "mapgrid.h"
#include "coordlabel.h"

#include "mapeditor.h"

const int default_width = 595;
const int default_height = 355;


MapEditor::MapEditor(QWidget *parent,Sprites **ss)
	: QWidget(parent)
{
	kDebug() << "sprites=" << *ss;

	setWindowTitle("Level Editor");
	setMinimumSize(default_width, default_height);
	resize(default_width, default_height);
	setSizeIncrement(Sprites::base_width,Sprites::base_height);

	map = NULL;
	sprites = ss;

	QGridLayout *gl = new QGridLayout(this);
	gl->setRowStretch(1,9);
	gl->setColumnStretch(5,9);
	gl->setColumnMinimumWidth(1,KDialog::spacingHint());
	gl->setColumnMinimumWidth(3,KDialog::spacingHint());
	gl->setRowMinimumHeight(2,KDialog::spacingHint());
	gl->setRowMinimumHeight(4,KDialog::marginHint());

	sprite_list = new ObjectListBox(false,this);
	sprite_list->setToolTip(i18n("The list of available objects to insert"));
	connect(sprite_list, SIGNAL(currentRowChanged(int)), SLOT(selectedSprite(int)));
	gl->addWidget(sprite_list,1,0);

	QLabel *label = new QLabel("Object &list:",this);
	label->setBuddy(sprite_list);
	gl->addWidget(label,0,0,Qt::AlignLeft);

	preview_sprite = new SpritePreview(this);
	preview_sprite->setToolTip(i18n("The current selected object"));
	gl->addWidget(preview_sprite,1,2,Qt::AlignTop);

	label = new QLabel("Object:", this);
	gl->addWidget(label,0,2,Qt::AlignLeft);

	optiongroup = new Q3ButtonGroup(1,Qt::Horizontal,"D&isplay",this);
	connect(optiongroup,SIGNAL(clicked(int)),this,SLOT(optionButton(int)));
	gl->addWidget(optiongroup,3,0,1,3,Qt::AlignLeft);

	(void) new QCheckBox("&Transporter routes",optiongroup);	// ID 0
	(void) new QCheckBox("S&elected transporter",optiongroup);	// ID 1

	map_area = new MapGrid(this);
	connect(map_area,SIGNAL(pressedButton(int,int,int)),
		this,SLOT(pressedButton(int,int,int)));
	connect(map_area,SIGNAL(changedCoordinates(int,int)),
		this,SLOT(updateCoordinates(int,int)));
	gl->addWidget(map_area,1,4,4,2);

	label = new QLabel("Ma&p:", this);
	label->setBuddy(map_area);
	gl->addWidget(label,0,4,Qt::AlignLeft);

#ifdef EDITOR_3_WINDOWS
	KPushButton *close_button = new KPushButton("&Close", this);
	close_button->setDefault(true);
	QToolTip::add(close_button, "Close this window");
	connect(close_button,SIGNAL(clicked()),this,SLOT(close()));
	gl->addWidget(close_button,5,0,Qt::AlignLeft);
#endif

	coord = new CoordLabel(this);
	gl->addWidget(coord,0,5,Qt::AlignRight);

        sprite_list->setFocus();
	selectedSprite(-1);
	optionButton(0);
	optionButton(1);

	kDebug() << "done";
}


void MapEditor::setMap(MapEdit *mm)
{
	kDebug() << "pw='" << (mm?mm->getPassword():QString("NULL")) << "'";

	map = mm;
	map_area->setMap(map);
	sprite_list->setEnabled(map!=NULL);
}

void MapEditor::selectedSprite(int i)
{
	kDebug() << "i=" << i;

	current_sprite = static_cast<Obj::Type>(i);
	preview_sprite->setSprite(*sprites,current_sprite);
	map_area->setSprite(*sprites,current_sprite);
}


void MapEditor::pressedButton(int button,int x,int y)
{
	if (map==NULL) return;
	if (current_sprite==static_cast<Obj::Type>(-1)) return;

	bool changed = false;
	if (button & Qt::LeftButton) changed = map->setCell(x,y,current_sprite);
	else if (button & Qt::RightButton) changed = map->setCell(x,y,Obj::Empty);
	if (!changed) return;

	map_area->updatedCell(x,y);
	emit modified(true);
}


void MapEditor::updateCoordinates(int x,int y)
{
	if (x>=0 && x<map->getWidth() && y>=0 && y<map->getHeight()) coord->setXY(x,y);
	else coord->clear();
}


void MapEditor::updateChilds()
{
	map_area->repaintContents(false);
}


void MapEditor::optionButton(int id)
{
	const QCheckBox *but = static_cast<const QCheckBox *>(optiongroup->find(id));
	switch (id)
	{
case 0:		map_area->showTransporters(but->isChecked());
		break;

case 1:		map_area->showSelectedTransporter(but->isChecked());
		break;
	}

	updateChilds();
}


void MapEditor::closeEvent(QCloseEvent *e)
{
	QWidget::closeEvent(e);
	hide();
	emit closed();
}


void MapEditor::selectedTransporter(int item)
{
	map_area->selectedTransporter(item);
	updateChilds();
}
