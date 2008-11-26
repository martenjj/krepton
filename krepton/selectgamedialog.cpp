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

#include <kiconview.h>

#include <qptrlist.h>
#include <qtimer.h>

#include "krepton.h"
#include "episodes.h"
#include "sprites.h"

#include "selectgamedialog.h"


SelectGameDialog::SelectGameDialog(const QString title,QWidget *parent, const char *name,bool useronly)
	: KDialogBase(parent,name,true,title,KDialogBase::Ok|KDialogBase::Cancel)
{
	u = new SelectGameUi(this);
	setMainWidget(u);

	u->episodeIconView->clear();
	u->episodeIconView->setItemsMovable(false);
	u->episodeIconView->setAutoArrange(true);

	episodes = EpisodeList::list();
	QPtrListIterator<Episode> ei(*episodes);
	for (const Episode *e; (e = ei.current())!=NULL; ++ei)
	{
		QIconViewItem *ic = new QIconViewItem(u->episodeIconView,e->getName());
		if (useronly && e->isGlobal()) ic->setSelectable(false);
	}

	u->episodeIconView->arrangeItemsInGrid(true);
	setFixedSize(calculateSize(u->size().width(),u->size().height()));
	adjustSize();

	connect(u->episodeIconView,SIGNAL(selectionChanged()),
		this,SLOT(slotSelectionChanged()));
	connect(u->episodeIconView,SIGNAL(doubleClicked(QIconViewItem *)),
		this,SLOT(slotExecuted(QIconViewItem *)));

	nexticon = 0;
	icontimer = new QTimer(this);
	connect(icontimer,SIGNAL(timeout()),this,SLOT(timerTick()));
	icontimer->start(100);

	slotSelectionChanged();
}


SelectGameDialog::~SelectGameDialog()
{
	icontimer->stop();
}


int SelectGameDialog::exec()
{
	int status = QDialog::exec();
	icontimer->stop();				// ensure stopped immediately
	return (status);
}


void SelectGameDialog::slotSelectionChanged()
{
	const QIconViewItem *sel = u->episodeIconView->currentItem();
	enableButtonOK(sel!=NULL && sel->isSelected());
}

const Episode *SelectGameDialog::selectedItem()
{
	const QIconViewItem *sel = u->episodeIconView->currentItem();
	if (sel==NULL) return (NULL);
	return (episodes->at(u->episodeIconView->index(sel)));
}

void SelectGameDialog::slotExecuted(QIconViewItem *item)
{
	slotOk();
}

void SelectGameDialog::timerTick()
{
//	kdDebug(0) << k_funcinfo << "next=" << nexticon << endl;
	if (nexticon>=episodes->count())		// everything is done
	{
		icontimer->stop();
		return;
	}

	QIconViewItem *it;
	int i = nexticon;
	for (it = u->episodeIconView->firstItem();
	     i>0 && it!=NULL; it = it->nextItem()) --i;

	if (it!=NULL && it->isSelectable())
	{
		QPixmap p = Sprites::preview(episodes->at(nexticon));
		if (!p.isNull()) it->setPixmap(p);
	}
	++nexticon;
}
