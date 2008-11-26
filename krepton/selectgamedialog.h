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

#ifndef SELECTGAMEDIALOG_H
#define SELECTGAMEDIALOG_H

#include <kdialogbase.h>

#include <qptrlist.h>

#include "episodes.h"

#include "selectgamewidgetui.h"

class QIconViewItem;

class SelectGameDialog : public KDialogBase
{
	Q_OBJECT

public:
	SelectGameDialog(const QString title,QWidget *parent = NULL,const char *name = NULL,bool useronly = false);
	~SelectGameDialog();
	const Episode *selectedItem();

public slots:
	int exec();

protected slots:
	void slotSelectionChanged();
	void slotExecuted(QIconViewItem *item);
	void timerTick();

private:
	SelectGameUi *u;

	QTimer *icontimer;
	unsigned int nexticon;
	EpisodeList *episodes;
};

#endif							// !SELECTGAMEDIALOG_H
