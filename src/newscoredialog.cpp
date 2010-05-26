////////////////////////////////////////////////////////////////////////////
//  
//  KRepton - the classic Repton game for KDE
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

#include <stdlib.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include <kdialogbase.h>

#include "newscoredialog.h"

NewScoreDialog::NewScoreDialog(QWidget *parent, const char *name)
	: KDialogBase(parent,name,true,"New High Score",
		      KDialogBase::Ok|KDialogBase::Cancel,KDialogBase::Ok)
{
	const char *username = getenv("LOGNAME");
	if (username==NULL) username = getenv("USER");
	if (username==NULL) username = "";

	QWidget *page = new QWidget(this);
	setMainWidget(page);
	Q3VBoxLayout *topLayout = new Q3VBoxLayout(page,0,spacingHint());

	le = new QLineEdit(username,page);
	le->setMinimumWidth(fontMetrics().maxWidth()*20);
	connect(le,SIGNAL(textChanged(const QString &)),
		this,SLOT(slotTextChanged(const QString &)));

	QLabel *label = new QLabel(le,"Enter your &name",page);

	topLayout->addWidget(label);
	topLayout->addWidget(le);
	topLayout->addStretch(10);

	slotTextChanged(username);
	le->setFocus();
}

const char *NewScoreDialog::name()
{
	const QString &t = le->text();
	return (!t.isEmpty() ? t : QString("Anonymous"));
}

void NewScoreDialog::slotTextChanged(const QString& text)
{
	enableButtonOK(text.length()>0);
}
