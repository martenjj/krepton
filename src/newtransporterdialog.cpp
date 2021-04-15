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

#include "newtransporterdialog.h"

#include <qgridlayout.h>
#include <qlabel.h>
#include <qspinbox.h>

#include "krepton.h"


NewTransporterDialog::NewTransporterDialog(const QString &title, QWidget *parent)
	: DialogBase(parent)
{
        setObjectName("NewTransporterDialog");
        setWindowTitle(title);
        setButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
        setModal(true);

	QWidget *w = new QWidget(this);
        QGridLayout *gl = new QGridLayout;

        mOrigXBox = new QSpinBox(w);
        mOrigYBox = new QSpinBox(w);
        QLabel *l = new QLabel(i18n("Origin:"), w);
        l->setBuddy(mOrigXBox);

        gl->addWidget(l, 0, 0, Qt::AlignRight);
        gl->addWidget(mOrigXBox, 0, 1, Qt::AlignLeft);
        gl->addWidget(new QLabel(i18n(","), w), 0, 2, Qt::AlignLeft);
        gl->addWidget(mOrigYBox, 0, 3, Qt::AlignLeft);

        mDestXBox = new QSpinBox(w);
        mDestYBox = new QSpinBox(w);
        l = new QLabel(i18n("Destination:"), w);
        l->setBuddy(mDestXBox);

        gl->addWidget(l, 2, 0, Qt::AlignRight);
        gl->addWidget(mDestXBox, 2, 1, Qt::AlignLeft);
        gl->addWidget(new QLabel(i18n(","), w), 2, 2, Qt::AlignLeft);
        gl->addWidget(mDestYBox, 2, 3, Qt::AlignLeft);

        gl->setColumnStretch(4, 1);
        gl->setRowMinimumHeight(1, DialogBase::verticalSpacing());
        gl->setRowStretch(3, 1);

        w->setLayout(gl);
	w->setMinimumSize(250, 70);
	setMainWidget(w);
}


void NewTransporterDialog::setLimits(int x,int y)
{
	mOrigXBox->setMaximum(x);
	mDestXBox->setMaximum(x);

	mOrigYBox->setMaximum(y);
	mDestYBox->setMaximum(y);
}


void NewTransporterDialog::setValues(int ox,int oy,int dx,int dy)
{
	mOrigXBox->setValue(ox);
	mDestXBox->setValue(dx);

	mOrigYBox->setValue(oy);
	mDestYBox->setValue(dy);
}


int NewTransporterDialog::xorig() const		{ return (mOrigXBox->value()); }
int NewTransporterDialog::yorig() const		{ return (mOrigYBox->value()); }
int NewTransporterDialog::xdest() const		{ return (mDestXBox->value()); }
int NewTransporterDialog::ydest() const		{ return (mDestYBox->value()); }
