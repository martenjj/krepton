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
#include <qgridlayout.h>
#include <qlistwidget.h>

#include <kpushbutton.h>
#include <kdialog.h>
#include <klineedit.h>
#include <knuminput.h>

#include "krepton.h"
#include "dataeditor.h"
#include "dataeditor.moc"


DataEditor::DataEditor(QWidget* parent)
	: QWidget(parent)
{
    setWindowTitle("Edit Episode");

    QGridLayout *l = new QGridLayout(this);

    QLabel *levelsLabel = new QLabel("&Levels:",this);
    l->addWidget(levelsLabel,1,1,1,2,Qt::AlignLeft);

    mapsListBox = new QListWidget(this);
    mapsListBox->setSelectionMode(QAbstractItemView::SingleSelection);
    levelsLabel->setBuddy(mapsListBox);
    l->addWidget(mapsListBox,2,1,6,2);

    newlevelPushButton = new KPushButton("&New...",this);
    l->addWidget(newlevelPushButton,2,4);

    removelevelPushButton = new KPushButton("&Remove",this);
    l->addWidget(removelevelPushButton,2,6);

    levelupPushButton = new KPushButton("Move &Up",this);
    l->addWidget(levelupPushButton,3,4);

    leveldownPushButton = new KPushButton("Move Do&wn",this);
    l->addWidget(leveldownPushButton,3,6);

    l->setRowMinimumHeight(4,KDialog::marginHint());

    QLabel *passwordLabel = new QLabel("&Password:",this);
    passwordLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    l->addWidget(passwordLabel,5,4,Qt::AlignRight);

    passwordLineEdit = new KLineEdit(this);
    passwordLineEdit->setMaxLength(20);
    passwordLabel->setBuddy(passwordLineEdit);
    l->addWidget(passwordLineEdit,5,5,1,2,Qt::AlignLeft);

    QLabel *timeLabel = new QLabel("T&ime limit:",this);
    timeLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    l->addWidget(timeLabel,6,4,Qt::AlignRight);

    timeSpinBox = new KIntSpinBox(this);
    timeSpinBox->setMaximum( 999 );
    timeSpinBox->setMinimum( 30 );
    timeSpinBox->setSingleStep( 10 );
    timeLabel->setBuddy(timeSpinBox);
    l->addWidget(timeSpinBox,6,5,1,2,Qt::AlignLeft);

    l->setRowStretch(7,1);

    QLabel *transportLabel = new QLabel("&Transporters:",this);
    l->addWidget(transportLabel,10,1,1,2,Qt::AlignLeft);

    transportListBox = new QListWidget(this);
    transportListBox->setSelectionMode(QAbstractItemView::SingleSelection);
    transportLabel->setBuddy(transportListBox);
    l->addWidget(transportListBox,11,1,3,2);

    newtransportPushButton = new KPushButton("N&ew...",this);
    l->addWidget(newtransportPushButton,11,4);

    removetransportPushButton = new KPushButton("Rem&ove",this);
    l->addWidget(removetransportPushButton,11,6);

    changetransportPushButton = new KPushButton("C&hange...",this);
    l->addWidget(changetransportPushButton,12,4);

    l->setRowStretch(13,1);

    l->setColumnStretch(7,1);
}
