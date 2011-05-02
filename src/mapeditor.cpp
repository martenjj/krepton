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
#include <qtooltip.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qgridlayout.h>

#include <kdialog.h>

#include "krepton.h"
#include "sprites.h"
#include "map2.h"
#include "objectlistbox.h"
#include "spritepreview.h"
#include "mapgrid.h"

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

        // TODO: persistent settings of these options
	QGroupBox *optiongroup = new QGroupBox("Display",this);
        QVBoxLayout *vl = new QVBoxLayout;

	QCheckBox *cb = new QCheckBox("Transporter routes",optiongroup);
	connect(cb,SIGNAL(toggled(bool)),SLOT(optionShowTransporterRoutes(bool)));
        vl->addWidget(cb);

	cb = new QCheckBox("Selected transporter",optiongroup);
	connect(cb,SIGNAL(toggled(bool)),SLOT(optionShowTransporterSelected(bool)));
        vl->addWidget(cb);

        optiongroup->setLayout(vl);
	gl->addWidget(optiongroup,3,0,1,3,Qt::AlignLeft);

	map_area = new MapGrid(this);
	connect(map_area,SIGNAL(pressedButton(int,int,int)),
		this,SLOT(pressedButton(int,int,int)));
	connect(map_area,SIGNAL(changedCoordinates(int,int)),
		SIGNAL(coordinatePosition(int,int)));
	gl->addWidget(map_area,1,4,4,2);

	label = new QLabel("Map:", this);
	label->setBuddy(map_area);
	gl->addWidget(label,0,4,Qt::AlignLeft);

        sprite_list->setFocus();
	selectedSprite(-1);

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
	map_area->setSprites(*sprites);
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


void MapEditor::updateChilds()
{
	map_area->update();
}


void MapEditor::optionShowTransporterRoutes(bool checked)
{
	map_area->showTransporters(checked);
	updateChilds();
}


void MapEditor::optionShowTransporterSelected(bool checked)
{
	map_area->showSelectedTransporter(checked);
	updateChilds();
}


void MapEditor::selectedTransporter(int item)
{
	map_area->selectedTransporter(item);
	updateChilds();
}
