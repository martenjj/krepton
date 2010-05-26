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
#include <qvbox.h>

#include "krepton.h"
#include "episodes.h"
#include "sprites.h"

#include "selectgamedialog.h"
#include "selectgamedialog.moc"


SelectGameDialog::SelectGameDialog(const QString title,QWidget *parent, const char *name,bool useronly)
	: KDialogBase(parent,name,true,title,KDialogBase::Ok|KDialogBase::Cancel)
{
    setCaption("Select Game");

    QVBox *vb = makeVBoxMainWidget();
    vb->setMargin(KDialog::marginHint());
    vb->setSpacing(KDialog::spacingHint());

    wEpisodeIconView = new QIconView(vb);
    wEpisodeIconView->clear();
    wEpisodeIconView->setItemsMovable(false);
    wEpisodeIconView->setAutoArrange(true);
    wEpisodeIconView->setResizeMode(QIconView::Adjust);
    wEpisodeIconView->setSpacing(10);
    wEpisodeIconView->setGridX(70);
    wEpisodeIconView->setGridY(50);
    wEpisodeIconView->setMaxItemTextLength(10);
    wEpisodeIconView->setWordWrapIconText(false);
    wEpisodeIconView->arrangeItemsInGrid(true);
    wEpisodeIconView->setMinimumSize(200,140);

    episodes = EpisodeList::list();
    QPtrListIterator<Episode> ei(*episodes);
    for (const Episode *e; (e = ei.current())!=NULL; ++ei)
    {
        QIconViewItem *ic = new QIconViewItem(wEpisodeIconView,e->getName());
        if (useronly && e->isGlobal()) ic->setSelectable(false);
    }

    connect(wEpisodeIconView,SIGNAL(selectionChanged()),
            this,SLOT(slotSelectionChanged()));
    connect(wEpisodeIconView,SIGNAL(doubleClicked(QIconViewItem *)),
            this,SLOT(slotExecuted(QIconViewItem *)));

    nexticon = 0;
    icontimer = new QTimer(this);
    connect(icontimer,SIGNAL(timeout()),this,SLOT(timerTick()));
    icontimer->start(50);

    resize(410,330);
    slotSelectionChanged();
}


SelectGameDialog::~SelectGameDialog()
{
    icontimer->stop();
}


int SelectGameDialog::exec()
{
    int status = QDialog::exec();
    icontimer->stop();					// ensure stopped immediately
    return (status);
}


void SelectGameDialog::slotSelectionChanged()
{
    const QIconViewItem *sel = wEpisodeIconView->currentItem();
    enableButtonOK(sel!=NULL && sel->isSelected());
}


const Episode *SelectGameDialog::selectedItem()
{
    const QIconViewItem *sel = wEpisodeIconView->currentItem();
    if (sel==NULL) return (NULL);
    return (episodes->at(wEpisodeIconView->index(sel)));
}


void SelectGameDialog::slotExecuted(QIconViewItem *item)
{
    slotOk();
}


void SelectGameDialog::timerTick()
{
    if (nexticon>=episodes->count())			// everything is done
    {
        icontimer->stop();
        return;
    }

    QIconViewItem *it;
    int i = nexticon;
    for (it = wEpisodeIconView->firstItem();
         i>0 && it!=NULL; it = it->nextItem()) --i;

    if (it!=NULL && it->isSelectable())
    {
        QPixmap p = Sprites::preview(episodes->at(nexticon));
        if (!p.isNull()) it->setPixmap(p);
    }
    ++nexticon;
}
