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

#ifndef NEWTRANSPORTERDIALOG_H
#define NEWTRANSPORTERDIALOG_H

#include <kfdialog/dialogbase.h>

class QSpinBox;


class NewTransporterDialog : public DialogBase
{
	Q_OBJECT

public:
	NewTransporterDialog(const QString &title, QWidget *parent = NULL);
	void setValues(int ox,int oy,int dx,int dy);
	void setLimits(int x,int y);

	int xorig() const;
	int yorig() const;
	int xdest() const;
	int ydest() const;

private:
        QSpinBox *mOrigXBox;
        QSpinBox *mOrigYBox;
        QSpinBox *mDestXBox;
        QSpinBox *mDestYBox;
};

#endif							// !NEWTRANSPORTERDIALOG_H
