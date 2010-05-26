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

#include <kdialogbase.h>
#include <knuminput.h>

#include "krepton.h"

#include "newtransporterdialog.h"


NewTransporterDialog::NewTransporterDialog(const QString &title,QWidget *parent,const char *name)
	: KDialogBase(parent,name,true,title,KDialogBase::Ok|KDialogBase::Cancel)
{
	w = new NewTransporterWidget(this);
	setMainWidget(w);
	setFixedSize(calculateSize(w->size().width(),w->size().height()));
	adjustSize();
}


void NewTransporterDialog::setLimits(int x,int y)
{
	w->origxSpinBox->setMaxValue(x);
	w->destxSpinBox->setMaxValue(x);

	w->origySpinBox->setMaxValue(y);
	w->destySpinBox->setMaxValue(y);
}


void NewTransporterDialog::setValues(int ox,int oy,int dx,int dy)
{
	w->origxSpinBox->setValue(ox);
	w->destxSpinBox->setValue(dx);

	w->origySpinBox->setValue(oy);
	w->destySpinBox->setValue(dy);
}
