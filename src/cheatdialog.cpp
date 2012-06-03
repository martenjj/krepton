////////////////////////// -*- indent-tabs-mode:t; c-basic-offset:8; -*- ///
//  
//  KRepton - the classic Repton game for KDE
//  
//  Copyright (c) 2012 Jonathan Marten <jjm@keelhaul.demon.co.uk>
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

#include "krepton.h"
#include "cheatdialog.h"
#include "cheatdialog.moc"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qcheckbox.h>
#include <qlabel.h>

#include <kdialog.h>
#include <kiconloader.h>
#include <kseparator.h>


CheatDialog::CheatDialog(const QString &title, QWidget *parent)
	: KDialog(parent)
{
	setObjectName("CheatDialog");
	setCaption(title);
	setButtons(KDialog::Ok|KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal(true);
	showButtonSeparator(true);

	QWidget *w = new QWidget(this);
	QVBoxLayout *vl = new QVBoxLayout;

	QHBoxLayout *hl = new QHBoxLayout;

	QLabel *l = new QLabel(w);
	l->setPixmap(KIconLoader::global()->loadIcon("task-attention",
						     KIconLoader::NoGroup,
						     KIconLoader::SizeMedium));
	hl->addWidget(l);

	l = new QLabel(i18n("Using any of these cheats will mean that your score for this episode will not be recorded in the high score list."), w);
	l->setWordWrap(true);
	hl->addWidget(l);
	vl->addLayout(hl);

	vl->addSpacing(KDialog::spacingHint());
	vl->addWidget(new KSeparator(Qt::Horizontal));
	vl->addSpacing(KDialog::spacingHint());

	noCrushCheck = new QCheckBox(i18n("Repton immune from falling objects"), w);
	noCrushCheck->setToolTip(i18n("If this option is turned on, Repton will not be crushed by falling objects (rocks or eggs)."));
	vl->addWidget(noCrushCheck);






	vl->addStretch(1);
	w->setLayout(vl);
	w->setMinimumSize(250, 70);
	setMainWidget(w);
}


void CheatDialog::setCheats(Cheat::Options cheats)
{
	kDebug() << "cheats" << cheats;

	noCrushCheck->setChecked(cheats & Cheat::CannotBeCrushed);



}


Cheat::Options CheatDialog::getCheats() const
{
	Cheat::Options cheats;

	if (noCrushCheck->isChecked()) cheats |= Cheat::CannotBeCrushed;





	kDebug() << "cheats" << cheats;
	return (cheats);
}
