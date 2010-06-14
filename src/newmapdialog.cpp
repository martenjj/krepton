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

#include "newmapdialog.h"
#include "newmapdialog.moc"

#include <stdlib.h>

#include <qlabel.h>
#include <qlayout.h>

#include "krepton.h"


NewMapDialog::NewMapDialog(QWidget *parent)
	: KDialog(parent)
{
	setObjectName("NewMapDialog");
        setCaption(i18n("New Level"));
        setButtons(KDialog::Ok|KDialog::Cancel);
        setDefaultButton(KDialog::Ok);
        setModal(true);
        showButtonSeparator(true);

        QWidget *w = new QWidget(this);

        QVBoxLayout *vb = new QVBoxLayout;

        QHBoxLayout *hb1 = new QHBoxLayout;

        QLabel *l = new QLabel(i18n("Name/Password:"), w);
        hb1->addWidget(l, 0, Qt::AlignRight);

        mNameEdit = new QLineEdit(w);
        connect(mNameEdit, SIGNAL(textChanged(const QString &)), SLOT(slotNameChanged(const QString &)));
        hb1->addWidget(mNameEdit, 1);
        l->setBuddy(mNameEdit);

        vb->addLayout(hb1);
        vb->addSpacing(KDialog::spacingHint());

        QHBoxLayout *hb2 = new QHBoxLayout;

        l = new QLabel(i18n("Width:"), w);
        hb2->addWidget(l, 0, Qt::AlignLeft);

        mSizeXBox = new QSpinBox(w);
        mSizeXBox->setMinimum(10);
        mSizeXBox->setMaximum(99);
        hb2->addWidget(mSizeXBox, 0, Qt::AlignLeft);
        l->setBuddy(mSizeXBox);

        hb2->addSpacing(KDialog::spacingHint());
        hb2->addStretch(1);

        l = new QLabel(i18n("Height:"), w);
        hb2->addWidget(l, 0, Qt::AlignRight);

        mSizeYBox = new QSpinBox(w);
        mSizeYBox->setMinimum(10);
        mSizeYBox->setMaximum(99);
        hb2->addWidget(mSizeYBox, 0, Qt::AlignRight);
        l->setBuddy(mSizeYBox);

        vb->addLayout(hb2);
        vb->addStretch(1);

        w->setLayout(vb);
	w->setMinimumSize(270, 70);
	setMainWidget(w);

	mNameEdit->setText(QString::number(rand() % 100000));
	mNameEdit->setFocus();
}


void NewMapDialog::slotNameChanged(const QString &s)
{
	enableButtonOk(!s.isEmpty());
}
