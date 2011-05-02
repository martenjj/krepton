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

#include "scoredialog.h"
#include "scoredialog.moc"

#include <qtreewidget.h>

#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>

#include "krepton.h"
#include "episodes.h"


const char *groupname = "High Score";


class ScoreListItem : public QTreeWidgetItem
{
public:
	ScoreListItem(QTreeWidget *parent) : QTreeWidgetItem(parent)	{}
	virtual bool operator<(const QTreeWidgetItem &other) const;
};


bool ScoreListItem::operator<(const QTreeWidgetItem &other) const
{
    int col = treeWidget()->sortColumn();
    QVariant v1 = this->data(col,Qt::UserRole);
    QVariant v2 = other.data(col,Qt::UserRole);

    if (v1.type()==QVariant::Int) return (v1.toInt()<v2.toInt());
    else return (v1.toString().toLower()<v2.toString().toLower());
}


// TODO: save/restore dialogue size and column layout

ScoreDialog::ScoreDialog(QWidget *parent)
        : KDialog(parent)
{
        setObjectName("ScoreDialog");
        setCaption(i18n("High Scores"));
        setButtons(KDialog::Close|KDialog::User1);
        setButtonGuiItem(KDialog::User1,KStandardGuiItem::clear());
        setDefaultButton(KDialog::Close);
        setModal(true);
        showButtonSeparator(true);

        connect(this,SIGNAL(user1Clicked()),SLOT(slotUser1()));

	configGrp = KGlobal::config()->group(groupname);

	list = new QTreeWidget(this);
	list->setSelectionMode(QAbstractItemView::NoSelection);
        list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list->setColumnCount(3);
        list->setRootIsDecorated(false);
        list->setAllColumnsShowFocus(true);
        list->setSortingEnabled(true);
        list->setMinimumWidth(350);

        QStringList hdrs;
        hdrs << i18n("Episode") << i18n("Score") << i18n("Name");
	list->setHeaderLabels(hdrs);

        int row = 0;
	const EpisodeList *el = EpisodeList::list();
	for (EpisodeList::const_iterator it = el->constBegin();
         it!=el->constEnd(); ++it)
	{
	        const Episode *e = (*it);
		const QString episode = e->getName().toUpper();
		if (episode=="---") continue;		// what does this do?

		int score = configGrp.readEntry((episode+"_Score"),0);
		if (score==0) score = configGrp.readEntry((episode+"Score"),0);
		if (score==0) continue;

		QString player = configGrp.readEntry((episode+"_Name"),"");
		if (player.isEmpty()) player = configGrp.readEntry((episode+"Name"),"");

                ScoreListItem *item = new ScoreListItem(list);
                item->setText(0,episode);
                item->setText(1,i18n("%1",score));	// formats with commas
                item->setText(2,player);

                item->setData(0,Qt::UserRole,episode);	// set sorting keys
                item->setData(1,Qt::UserRole,score);
                item->setData(2,Qt::UserRole,player);

                ++row;
	}

	setMainWidget(list);

	if (row>0)					// some content in table
        {						// now can resize columns
            for (int col = 0; col<=2; ++col) list->resizeColumnToContents(col);
            list->sortItems(1,Qt::DescendingOrder);	// set initial sort state
        }
        else						// nothing in the list
        {
            enableButton(KDialog::User1,false);		// no point in this button
        }
}


void ScoreDialog::slotUser1()
{
	if (KMessageBox::warningContinueCancel(
		    this,"Do you really want to clear the high scores list?",
		    QString::null,KStandardGuiItem::clear())!=KMessageBox::Continue) return;

	configGrp.config()->deleteGroup(groupname);	// deep delete group

	list->clear();					// nothing now to display
	enableButton(KDialog::User1,false);		// can't clear again
}
