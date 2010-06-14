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

#include "saveepisodedialog.h"
#include "saveepisodedialog.moc"

#include <qlineedit.h>
#include <qformlayout.h>

#include <kdialog.h>
#include <kstandardguiitem.h>
#include <ksqueezedtextlabel.h>

#include "krepton.h"
#include "episodes.h"


SaveEpisodeDialog::SaveEpisodeDialog(const QString &title, QWidget *parent)
	: KDialog(parent)
{
	setObjectName("SaveEpisodeDialog");
        setCaption(title);
        setButtons(KDialog::Ok|KDialog::Cancel);
        setDefaultButton(KDialog::Ok);
        setModal(true);
        showButtonSeparator(true);

        QWidget *w = new QWidget(this);
        QFormLayout *fl = new QFormLayout;

        mNameEdit = new QLineEdit(w);
        connect(mNameEdit, SIGNAL(textChanged(const QString &)), SLOT(slotNameChanged(const QString &)));
        fl->addRow(i18n("Epsiode name:"), mNameEdit);

        mSaveLocation = new KSqueezedTextLabel(w);
        mSaveLocation->setTextElideMode(Qt::ElideMiddle); 
        fl->addRow(i18n("Save location:"), mSaveLocation);

        w->setLayout(fl);
	setMainWidget(w);
	setMinimumSize(420, 120);

	setButtonGuiItem(KDialog::Ok, KStandardGuiItem::save());

	mNameEdit->clear();
	slotNameChanged("");
	enableButtonOk(false);
        mNameEdit->setFocus();
}


void SaveEpisodeDialog::slotNameChanged(const QString &s)
{
        QString path = s;
        if (!path.isEmpty()) path = Episode::savePath(s);
        mSaveLocation->setText(path);

        mSaveLocation->setEnabled(!s.isEmpty());
        enableButtonOk(!s.isEmpty());
}
