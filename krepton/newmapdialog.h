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

#ifndef NEWMAPDIALOG_H
#define NEWMAPDIALOG_H

#include <kdialogbase.h>
#include <knuminput.h>
#include <klineedit.h>

#include "newmapwidgetui.h"

class NewMapDialog : public KDialogBase
{
	Q_OBJECT

public:
	NewMapDialog(QWidget *parent = 0, const char *name = 0);
	int mapWidth() { return (w->sizexSpinBox->value()); }
	int mapHeight() { return (w->sizeySpinBox->value()); }
	QString mapPassword() { return (w->nameLineEdit->text()); }

protected slots:
	void slotNameChanged(const QString &s);

private:
	NewMapWidget *w;
};

#endif							// !NEWMAPDIALOG_H
