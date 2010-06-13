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

#include <q3iconview.h>
#include <q3ptrlist.h>
#include <qtimer.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <QPixmap>

#include "krepton.h"
#include "episodes.h"
#include "sprites.h"

#include "selectgamedialog.h"
#include "selectgamedialog.moc"


SelectGameDialog::SelectGameDialog(const QString title, QWidget *parent, bool useronly)
	: KDialog(parent)
{
    setObjectName("SelectGameDialog");
    setCaption(title);
    setButtons(KDialog::Ok|KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setModal(true);
    showButtonSeparator(true);

    Q3VBox *vb = new Q3VBox(this);
    setMainWidget(vb);
    vb->setMargin(KDialog::marginHint());
    vb->setSpacing(KDialog::spacingHint());

    wEpisodeIconView = new Q3IconView(vb);
    wEpisodeIconView->clear();
    wEpisodeIconView->setItemsMovable(false);
    wEpisodeIconView->setAutoArrange(true);
    wEpisodeIconView->setResizeMode(Q3IconView::Adjust);
    wEpisodeIconView->setSpacing(10);
    wEpisodeIconView->setGridX(70);
    wEpisodeIconView->setGridY(50);
    wEpisodeIconView->setMaxItemTextLength(10);
    wEpisodeIconView->setWordWrapIconText(false);
    wEpisodeIconView->arrangeItemsInGrid(true);
    wEpisodeIconView->setMinimumSize(200,140);

    episodes = EpisodeList::list();
    Q3PtrListIterator<Episode> ei(*episodes);
    for (const Episode *e; (e = ei.current())!=NULL; ++ei)
    {
        Q3IconViewItem *ic = new Q3IconViewItem(wEpisodeIconView,e->getName());
        if (useronly && e->isGlobal()) ic->setSelectable(false);
    }

    connect(wEpisodeIconView,SIGNAL(selectionChanged()),
            this,SLOT(slotSelectionChanged()));
    connect(wEpisodeIconView,SIGNAL(doubleClicked(Q3IconViewItem *)),
            this,SLOT(slotExecuted(Q3IconViewItem *)));

    nexticon = 0;
    // TODO: icontimer is never deleted, do in destructor?
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
    const Q3IconViewItem *sel = wEpisodeIconView->currentItem();
    enableButtonOk(sel!=NULL && sel->isSelected());
}


const Episode *SelectGameDialog::selectedItem()
{
    const Q3IconViewItem *sel = wEpisodeIconView->currentItem();
    if (sel==NULL) return (NULL);
    return (episodes->at(wEpisodeIconView->index(sel)));
}


void SelectGameDialog::slotExecuted(Q3IconViewItem *item)
{
// TODO: port
//    slotOk();
}


void SelectGameDialog::timerTick()
{
    if (nexticon>=episodes->count())			// everything is done
    {
        icontimer->stop();
        return;
    }

    Q3IconViewItem *it;
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
