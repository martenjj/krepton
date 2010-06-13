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

#include <klineedit.h>
#include <knuminput.h>

#include <stdlib.h>

#include "krepton.h"

#include "newmapdialog.h"


NewMapDialog::NewMapDialog(QWidget *parent)
	: KDialog(parent)
{
	setObjectName("NewMapDialog");
        setCaption(i18n("New Level"));
        setButtons(KDialog::Ok|KDialog::Cancel);
        setDefaultButton(KDialog::Ok);
        setModal(true);
        showButtonSeparator(true);

	w = new NewMapWidget(this);
	setMainWidget(w);
        // TODO: port
	//setFixedSize(calculateSize(w->size().width(),w->size().height()));
	adjustSize();

	w->sizexSpinBox->setMinimum(10);
	w->sizeySpinBox->setMinimum(10);
	w->sizexSpinBox->setMaximum(99);
	w->sizeySpinBox->setMaximum(99);

	QString s;
	s.setNum(rand() % 100000);
	w->nameLineEdit->setText(s);

	connect(w->nameLineEdit,SIGNAL(textChanged(const QString&)),
		this,SLOT(slotNameChanged(const QString&)));
	w->nameLineEdit->setFocus();
}


void NewMapDialog::slotNameChanged(const QString &s)
{
	enableButtonOk(!s.isEmpty());
}
