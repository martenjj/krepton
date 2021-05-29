////////////////////////////////////////////////////// -*- mode:c++; -*- ///
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

#ifndef GAMEEDITOR_H
#define GAMEEDITOR_H

#include <kxmlguiwindow.h>

#include "krepton.h"
#include "sprites.h"
#include "mapedit.h"


class DataEditor;
class SpriteEditor;
class MapEditor;

class QTabWidget;
class QAction;
class QPushButton;
class QLabel;


class GameEditor : public KXmlGuiWindow
{
	Q_OBJECT

public:
	GameEditor(QWidget *parent = NULL);
	~GameEditor();

	void startEdit(const QString name,const MapList ml,const Sprites *ss);
	bool isModified() const { return (modified); }
	const Sprites *getSprites() { return (sprites); }
	const MapList getMaps() { return (maps); }

public slots:
	void menuStrictCheck();
        void showSpriteEditor();
        void showLevelEditor();
        void showDataEditor();
	void setModified(bool mod = true);

signals:
	void editMapsChange();
	void editModified();

protected:
	void closeEvent(QCloseEvent *e) override;

protected slots:
	void mapInsert();
	void mapRemove();
	void mapMoveUp();
	void mapMoveDown();
	void transporterInsert();
	void transporterChange();
	void transporterRemove();
	void selectedMap();
	void selectedTransporter();
	void changedPassword(const QString &s);
	void changedTime(int i);
	void changedSprite();
	void slotShowCoordinates(int x,int y);

private:
	DataEditor *view;
	SpriteEditor *spritewin;
	MapEditor *mapwin;

	QTabWidget *tabs;
	QPushButton* checkPushButton;
	QPushButton* closePushButton;
        int dataIndex;
        int mapIndex;
        int spriteIndex;

	QAction *spriteAction;
	QAction *mapAction;
	QAction *dataAction;
	QAction *checkAction;

	QString epname;
	MapEditList maps;
	Sprites *sprites;

	bool modified;
	QLabel *coordsLabel;

	void updateCaption();
	void selectLevel(int level);
	void updateMapsList();
	void updateTransportersList(int item = -1);
};

#endif							// !GAMEEDITOR_H
