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

#ifndef DATAEDITOR_H
#define DATAEDITOR_H

#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class KListBox;
class QListBoxItem;
class KPushButton;
class QLabel;
class KLineEdit;
class KIntSpinBox;

class DataEditor : public QWidget
{
    Q_OBJECT

public:
    DataEditor(QWidget* parent = NULL,const char *name = NULL);

    KListBox* mapsListBox;
    KListBox* transportListBox;

    KPushButton* newlevelPushButton;
    KPushButton* removelevelPushButton;
    KPushButton* levelupPushButton;
    KPushButton* leveldownPushButton;

    KIntSpinBox* timeSpinBox;
    KLineEdit* passwordLineEdit;

    KPushButton* newtransportPushButton;
    KPushButton* changetransportPushButton;
    KPushButton* removetransportPushButton;
};

#endif							// DATAEDITOR_H
