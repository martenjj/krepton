////////////////////////////////////////////////////// -*- mode:c++; -*- ///
//  
//  KRepton - the classic Repton game for KDE
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

#ifndef SCOREDIALOG_H
#define SCOREDIALOG_H

#include <dialogbase.h>
#include <kconfiggroup.h>

class QTreeWidget;


class ScoreDialog : public DialogBase
{
	Q_OBJECT

public:
	ScoreDialog(QWidget *parent = NULL);

protected slots:
	void slotUser1();

private:
	QTreeWidget *list;
	KConfigGroup configGrp;
};

#endif							// !SCOREDIALOG_H
