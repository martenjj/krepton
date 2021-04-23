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

#include "cheatdialog.h"

#include <qboxlayout.h>
#include <qcheckbox.h>

#include <kmessagewidget.h>

#include "krepton.h"


CheatDialog::CheatDialog(const QString &title, QWidget *parent)
	: DialogBase(parent)
{
	setObjectName("CheatDialog");
	setWindowTitle(title);
	setButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
	setModal(true);

	QWidget *w = new QWidget(this);
	QVBoxLayout *vl = new QVBoxLayout;

	noCrushCheck = new QCheckBox(i18n("Immune from &falling objects"), w);
	noCrushCheck->setToolTip(i18n("If this option is turned on, Repton will not be crushed by falling objects (rocks or eggs)."));
	vl->addWidget(noCrushCheck);

	noTimeCheck = new QCheckBox(i18n("No &time limit"), w);
	noTimeCheck->setToolTip(i18n("If this option is turned on, there will be no time limit to the game"));
	vl->addWidget(noTimeCheck);

	noReplicatingPlantCheck = new QCheckBox(i18n("Plants do not reproduce"), w);
	noReplicatingPlantCheck->setToolTip(i18n("If this option is turned on, plants (or the equivalent) do not reproduce (but they are still fatal to touch)"));
	vl->addWidget(noReplicatingPlantCheck);

	harmlessPlantCheck = new QCheckBox(i18n("Plants are harmless"), w);
	harmlessPlantCheck->setToolTip(i18n("If this option is turned on, plants (or the equivalent) are not dangerous (but they still reproduce)"));
	vl->addWidget(harmlessPlantCheck);

	harmlessSkullCheck = new QCheckBox(i18n("Skulls are harmless"), w);
	harmlessSkullCheck->setToolTip(i18n("If this option is turned on, skulls (or the equivalent) are not dangerous"));
	vl->addWidget(harmlessSkullCheck);

	harmlessMonsterCheck = new QCheckBox(i18n("Monsters are harmless"), w);
	harmlessMonsterCheck->setToolTip(i18n("If this option is turned on, monsters are not dangerous (but they can still be killed)"));
	vl->addWidget(harmlessMonsterCheck);

	harmlessSpiritCheck = new QCheckBox(i18n("Spirits are harmless"), w);
	harmlessSpiritCheck->setToolTip(i18n("If this option is turned on, spirits are not dangerous (but they can still be caught)"));
	vl->addWidget(harmlessSpiritCheck);

	vl->addSpacing(DialogBase::verticalSpacing());
	vl->addStretch(1);

	KMessageWidget *mw = new KMessageWidget(w);
	mw->setMessageType(KMessageWidget::Information);
	mw->setText(i18n("Using any of these cheats will mean that your score for this episode will not be recorded in the high score list."));
	mw->setIcon(QIcon::fromTheme("task-attention"));
	mw->setWordWrap(true);
	mw->setCloseButtonVisible(false);
	vl->addWidget(mw);

	w->setLayout(vl);
	w->setMinimumSize(420, 280);
	setMainWidget(w);
}


void CheatDialog::setCheats(Cheat::Options cheats)
{
	qDebug() << "cheats" << cheats;

	noCrushCheck->setChecked(cheats & Cheat::CannotBeCrushed);
	noTimeCheck->setChecked(cheats & Cheat::NoTimeLimit);
	noReplicatingPlantCheck->setChecked(cheats & Cheat::NoReplicatingPlant);
	harmlessPlantCheck->setChecked(cheats & Cheat::HarmlessPlant);
	harmlessSkullCheck->setChecked(cheats & Cheat::HarmlessSkull);
	harmlessMonsterCheck->setChecked(cheats & Cheat::HarmlessMonster);
	harmlessSpiritCheck->setChecked(cheats & Cheat::HarmlessSpirit);
}


Cheat::Options CheatDialog::getCheats() const
{
	Cheat::Options cheats;

	if (noCrushCheck->isChecked()) cheats |= Cheat::CannotBeCrushed;
	if (noTimeCheck->isChecked()) cheats |= Cheat::NoTimeLimit;
	if (noReplicatingPlantCheck->isChecked()) cheats |= Cheat::NoReplicatingPlant;
	if (harmlessPlantCheck->isChecked()) cheats |= Cheat::HarmlessPlant;
	if (harmlessSkullCheck->isChecked()) cheats |= Cheat::HarmlessSkull;
	if (harmlessMonsterCheck->isChecked()) cheats |= Cheat::HarmlessMonster;
	if (harmlessSpiritCheck->isChecked()) cheats |= Cheat::HarmlessSpirit;

	qDebug() << "cheats" << cheats;
	return (cheats);
}
