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

#include <kmainwindow.h>

#include "krepton.h"
#include "sprites.h"
#include "map2.h"
//Added by qt3to4:
#include <QMoveEvent>
#include <QCloseEvent>

class DataEditor;
class SpriteEditor;
class MapEditor;
class KXMLGUIClient;
#ifndef EDITOR_3_WINDOWS
class QTabWidget;
class KPushButton;
#endif

class GameEditor : public KMainWindow
{
	Q_OBJECT

public:
	GameEditor(KXMLGUIClient *parent = NULL,const char *name = NULL);
	~GameEditor();

	void startEdit(const QString name,const MapList ml,const Sprites *ss);
	bool isModified() const { return (modified); }
	const Sprites *getSprites() { return (sprites); }
	const MapList getMaps() { return (maps); }

	bool spriteVisible() const;
	bool mapVisible() const;
	void setEnabled(bool enable);
	void setAlign(bool enable) { align = enable; }

public slots:
	void menuStrictCheck();
        void showSpriteEditor(bool show);
        void showLevelEditor(bool show);
        void showDataEditor(bool show);
	void menuRealign();
	void setModified(bool mod = true);

signals:
	void editWindowChange();
	void editMapsChange();
	void editModified();
	void closed();

protected:
	void moveEvent(QMoveEvent *e);
	void closeEvent(QCloseEvent *e);

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
	void updateWindowStates();

private:
	DataEditor *view;
	SpriteEditor *spritewin;
	MapEditor *mapwin;
#ifndef EDITOR_3_WINDOWS
	QTabWidget *tabs;
	KPushButton* checkPushButton;
	KPushButton* closePushButton;
#endif

	QString epname;
	MapEditList maps;
	Sprites *sprites;

	QString caption;
	bool modified;
	bool align;

	void updateCaption();
	void selectLevel(int level);
	void updateMapsList();
	void updateTransportersList(int item = -1);
	void alignWindows();
};

#endif							// !GAMEEDITOR_H
