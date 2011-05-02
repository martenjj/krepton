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

#include "selectgamedialog.h"
#include "selectgamedialog.moc"

#include <qlistwidget.h>
#include <qtimer.h>
#include <qpixmap.h>

#include "krepton.h"
#include "episodes.h"
#include "sprites.h"


SelectGameDialog::SelectGameDialog(const QString title, QWidget *parent, bool useronly)
	: KDialog(parent)
{
    setObjectName("SelectGameDialog");
    setCaption(title);
    setButtons(KDialog::Ok|KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setModal(true);
    showButtonSeparator(true);

    wEpisodeIconView = new QListWidget(this);
    setMainWidget(wEpisodeIconView);

    wEpisodeIconView->clear();
    wEpisodeIconView->setViewMode(QListView::IconMode);
    wEpisodeIconView->setMovement(QListView::Static);
    //wEpisodeIconView->setUniformItemSizes(true);
    wEpisodeIconView->setResizeMode(QListView::Adjust);
    wEpisodeIconView->setSpacing(10);
    wEpisodeIconView->setGridSize(QSize(75,60));
    wEpisodeIconView->setMinimumSize(340,240);

    episodes = EpisodeList::list();
    for (EpisodeList::const_iterator it = episodes->constBegin();
         it!=episodes->constEnd(); ++it)
    {
        const Episode *e = (*it);
        QListWidgetItem *item = new QListWidgetItem(KIcon("folder-grey"),e->getName());
        if (useronly && e->isGlobal()) item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        wEpisodeIconView->addItem(item);
    }

    connect(wEpisodeIconView,SIGNAL(itemSelectionChanged()),
            this,SLOT(slotSelectionChanged()));
    connect(wEpisodeIconView,SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this,SLOT(slotExecuted(QListWidgetItem *)));

    nexticon = 0;
    icontimer = new QTimer(this);
    connect(icontimer,SIGNAL(timeout()),this,SLOT(timerTick()));
    icontimer->start(50);

    slotSelectionChanged();
}


SelectGameDialog::~SelectGameDialog()
{
    icontimer->stop();
}


int SelectGameDialog::exec()
{
    int status = QDialog::exec();
    icontimer->stop();					// ensure stopped on close
    return (status);
}


void SelectGameDialog::slotSelectionChanged()
{
    const QListWidgetItem *sel = wEpisodeIconView->currentItem();
    enableButtonOk(sel!=NULL && sel->isSelected());
}


const Episode *SelectGameDialog::selectedItem()
{
    const QListWidgetItem *sel = wEpisodeIconView->currentItem();
    if (sel==NULL) return (NULL);
    return (episodes->at(wEpisodeIconView->row(sel)));
}


void SelectGameDialog::slotExecuted(QListWidgetItem *item)
{
    accept();
}


void SelectGameDialog::timerTick()
{
    if (nexticon>=episodes->count())			// everything is done
    {
        icontimer->stop();
        return;
    }

    for (;;)
    {
        QListWidgetItem *item = wEpisodeIconView->item(nexticon);
        ++nexticon;
        if (item==NULL) break;

        if (item->flags() & Qt::ItemIsEnabled)
        {
            QPixmap p = Sprites::preview(episodes->at(nexticon-1));
            if (!p.isNull()) item->setIcon(p);
            break;
        }
    }
}

