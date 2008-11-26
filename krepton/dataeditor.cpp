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

#include "config.h"

#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <kpushbutton.h>
#include <klistbox.h>
#include <kdialog.h>
#include <klineedit.h>
#include <knuminput.h>

#include "krepton.h"
#include "dataeditor.h"


DataEditor::DataEditor(QWidget* parent,const char* name)
	: QWidget(parent,name)
{
    setCaption("Edit Episode");

    const int kmh = KDialog::marginHint();
    const int ksh = KDialog::spacingHint();

    QGridLayout *l = new QGridLayout(this,16,8,kmh,ksh);

    QLabel *transportLabel = new QLabel("&Transporters:",this);
    l->addMultiCellWidget(transportLabel,10,10,1,2,Qt::AlignLeft);

    transportListBox = new KListBox(this);
    l->addMultiCellWidget(transportListBox,11,14,1,2);

    removetransportPushButton = new KPushButton("Rem&ove",this);
    l->addWidget(removetransportPushButton,11,6);

    changetransportPushButton = new KPushButton("C&hange...",this);
    l->addWidget(changetransportPushButton,13,4);

    newtransportPushButton = new KPushButton("N&ew...",this);
    l->addWidget(newtransportPushButton,11,4);

    QLabel *levelsLabel = new QLabel("&Levels:",this);
    l->addMultiCellWidget(levelsLabel,1,1,1,2,Qt::AlignLeft);

    passwordLineEdit = new KLineEdit(this);
    passwordLineEdit->setMaxLength(20);
    l->addMultiCellWidget(passwordLineEdit,7,7,5,6,Qt::AlignLeft);

    QLabel *passwordLabel = new QLabel("&Password:",this);
    passwordLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    l->addWidget(passwordLabel,7,4,Qt::AlignRight);

    mapsListBox = new KListBox(this);
    l->addMultiCellWidget(mapsListBox,2,5,1,2);

    timeSpinBox = new KIntSpinBox(this);
    timeSpinBox->setMaxValue( 999 );
    timeSpinBox->setMinValue( 30 );
    timeSpinBox->setLineStep( 10 );
    l->addWidget(timeSpinBox,7,2,Qt::AlignLeft);

    QLabel *timeLabel = new QLabel("T&ime limit:",this);
    timeLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    l->addWidget(timeLabel,7,1,Qt::AlignRight);

    leveldownPushButton = new KPushButton("Move Do&wn",this);
    l->addWidget(leveldownPushButton,4,6);

    levelupPushButton = new KPushButton("Move &Up",this);
    l->addWidget(levelupPushButton,4,4);

    newlevelPushButton = new KPushButton("&New...",this);
    l->addWidget(newlevelPushButton,2,4);

    removelevelPushButton = new KPushButton("&Remove",this);
    l->addWidget(removelevelPushButton,2,6);

    levelsLabel->setBuddy(mapsListBox);
    transportLabel->setBuddy(transportListBox);
    passwordLabel->setBuddy(passwordLineEdit);
    timeLabel->setBuddy(timeSpinBox);

    l->setRowStretch(5,1);
    l->setRowStretch(14,1);

    l->setColStretch(7,1);

    l->addRowSpacing(0,ksh);
    l->addRowSpacing(3,ksh);
    l->addRowSpacing(6,ksh);
    l->addRowSpacing(8,ksh);
    l->addRowSpacing(9,kmh);
    l->addRowSpacing(10,ksh);
    l->addRowSpacing(12,ksh);
    l->addRowSpacing(15,ksh);

    l->addColSpacing(0,ksh);
    l->addColSpacing(3,ksh);
    l->addColSpacing(5,ksh);
}


#include "dataeditor.moc"
