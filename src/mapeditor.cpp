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

#include "mapeditor.h"

#include <qlabel.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qgridlayout.h>

#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <dialogbase.h>

#include "krepton.h"
#include "sprites.h"
#include "mapedit.h"
#include "objectlistbox.h"
#include "spritepreview.h"
#include "mapgrid.h"

const int default_width = 595;
const int default_height = 355;


MapEditor::MapEditor(QWidget *parent)
	: QWidget(parent)
{
	qDebug();

	setObjectName("MapEditor");
	setWindowTitle("Level Editor");
	setMinimumSize(default_width, default_height);
	resize(default_width, default_height);
	setSizeIncrement(Sprites::base_width,Sprites::base_height);

	map = NULL;
	sprites = NULL;

	QGridLayout *gl = new QGridLayout(this);
	gl->setRowStretch(1,9);
	gl->setColumnStretch(5,9);
	gl->setColumnMinimumWidth(1,DialogBase::horizontalSpacing());
	gl->setColumnMinimumWidth(3,DialogBase::horizontalSpacing());
	gl->setRowMinimumHeight(2,DialogBase::verticalSpacing());
	gl->setRowMinimumHeight(4,2*DialogBase::verticalSpacing());

	sprite_list = new ObjectListBox(false, this);
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

	map_area = new MapGrid(this);
	connect(map_area,SIGNAL(pressedButton(int,int,int)),
		this,SLOT(pressedButton(int,int,int)));
	connect(map_area,SIGNAL(changedCoordinates(int,int)),
		SIGNAL(coordinatePosition(int,int)));
	gl->addWidget(map_area,1,4,4,2);

	label = new QLabel("Map:", this);
	label->setBuddy(map_area);
	gl->addWidget(label,0,4,Qt::AlignLeft);

	const KConfigGroup grp = KSharedConfig::openConfig()->group(objectName());
	QGroupBox *optiongroup = new QGroupBox("Display",this);
        QVBoxLayout *vl = new QVBoxLayout;

	QCheckBox *cb = new QCheckBox("Transporter routes",optiongroup);
	cb->setChecked(grp.readEntry("ShowTransporterRoutes", false));
	map_area->showTransporters(cb->isChecked());
	connect(cb,SIGNAL(toggled(bool)),SLOT(optionShowTransporterRoutes(bool)));
	vl->addWidget(cb);

	cb = new QCheckBox("Selected transporter",optiongroup);
	cb->setChecked(grp.readEntry("ShowTransporterSelected", false));
	map_area->showSelectedTransporter(cb->isChecked());
	connect(cb,SIGNAL(toggled(bool)),SLOT(optionShowTransporterSelected(bool)));
	vl->addWidget(cb);

	cb = new QCheckBox("Spirit routes",optiongroup);
	cb->setChecked(grp.readEntry("ShowSpiritRoutes", false));
	map_area->showSpiritRoutes(cb->isChecked());
	connect(cb,SIGNAL(toggled(bool)),SLOT(optionShowSpiritRoutes(bool)));
	vl->addWidget(cb);

        optiongroup->setLayout(vl);
	gl->addWidget(optiongroup,3,0,1,3,Qt::AlignLeft);

        sprite_list->setFocus();
	selectedSprite(-1);

	qDebug() << "done";
}


void MapEditor::setMap(MapEdit *mm)
{
	qDebug() << "pw" << (mm!=NULL ? mm->getPassword() : "(null)");

	map = mm;
	map_area->setMap(map);
	sprite_list->setEnabled(map!=NULL);
}


void MapEditor::setSprites(const Sprites *ss)
{
	sprites = ss;
	map_area->setSprites(sprites);
	sprite_list->setSprites(sprites);
	preview_sprite->setSprite(sprites,current_sprite);
}


void MapEditor::selectedSprite(int i)
{
	qDebug() << i;
	current_sprite = static_cast<Obj::Type>(i);
	preview_sprite->setSprite(sprites,current_sprite);
}


void MapEditor::pressedButton(int button,int x,int y)
{
	if (map==NULL) return;
	if (current_sprite==Obj::None) return;

	bool changed = false;
	if (button & Qt::LeftButton) changed = map->setCell(x,y,current_sprite);
	else if (button & Qt::RightButton) changed = map->setCell(x,y,Obj::Empty);
	if (!changed) return;

	map_area->updatedCell(x,y);
	emit modified(true);
}


void MapEditor::optionShowTransporterRoutes(bool checked)
{
	map_area->showTransporters(checked);

	KConfigGroup grp = KSharedConfig::openConfig()->group(objectName());
	grp.writeEntry("ShowTransporterRoutes", checked);
	grp.sync();
}


void MapEditor::optionShowSpiritRoutes(bool checked)
{
	map_area->showSpiritRoutes(checked);

	KConfigGroup grp = KSharedConfig::openConfig()->group(objectName());
	grp.writeEntry("ShowSpiritRoutes", checked);
	grp.sync();
}


void MapEditor::optionShowTransporterSelected(bool checked)
{
	map_area->showSelectedTransporter(checked);

	KConfigGroup grp = KSharedConfig::openConfig()->group(objectName());
	grp.writeEntry("ShowTransporterSelected", checked);
	grp.sync();
}


void MapEditor::selectedTransporter(int item)
{
	map_area->selectedTransporter(item);
}
