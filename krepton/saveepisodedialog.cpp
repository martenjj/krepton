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
#include <kstdguiitem.h>

#include "krepton.h"
#include "episodes.h"

#include "saveepisodedialog.h"


SaveEpisodeDialog::SaveEpisodeDialog(const QString &title,QWidget *parent,const char *name)
	: KDialogBase(parent,name,true,title,KDialogBase::Ok|KDialogBase::Cancel)
{
	w = new SaveEpisodeWidget(this);
	setMainWidget(w);
	setFixedSize(calculateSize(w->size().width(),w->size().height()));
	adjustSize();

	setButtonOKText(KStdGuiItem::save().text());

        connect(w->nameLineEdit,SIGNAL(textChanged(const QString&)),
                this,SLOT(slotNameChanged(const QString&)));

	w->nameLineEdit->clear();
	slotNameChanged("");
	enableButtonOK(false);
        w->nameLineEdit->setFocus();
}


void SaveEpisodeDialog::slotNameChanged(const QString &s)
{
	if (s.isNull()) fullpath = s;
	else fullpath = Episode::savePath(s);
	w->pathSqueezedTextLabel->setText(fullpath);

	w->pathSqueezedTextLabel->setEnabled(!s.isEmpty());
	enableButtonOK(!s.isEmpty());
}
