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

#include <kglobal.h>
#include <kconfig.h>
#include <klistview.h>
#include <kmessagebox.h>

#include <q3header.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include "krepton.h"
#include "episodes.h"

#include "scoredialog.h"


const char *groupname = "High Score";


class ScoreListItem : public KListViewItem
{
public:
	ScoreListItem(Q3ListView *parent,QString col1,QString col2,QString col3);
	int compare(Q3ListViewItem *i,int col,bool ascending) const;
};

ScoreListItem::ScoreListItem(Q3ListView *parent,QString col1,QString col2,QString col3)
	: KListViewItem(parent,col1,col2,col3)
{
}

int ScoreListItem::compare(Q3ListViewItem *i,int col,bool ascending) const
{
	if (col!=1) return (key(col,ascending).compare(i->key(col,ascending)));

	int n1 = key(col,ascending).toInt();
	int n2 = i->key(col,ascending).toInt();
	return ((n1==n2) ? 0 : (n1<n2 ? -1 : +1));
}

ScoreDialog::ScoreDialog(QWidget *parent,const char *name)
        : KDialogBase(parent,name,true,"High Scores",
KDialogBase::Close|KDialogBase::User1,
KDialogBase::Close,
false,
KGuiItem("C&lear")
)
{
	config = KGlobal::config();
	config->setGroup(groupname);

	QWidget *page = new QWidget(this);
	Q3VBoxLayout *vl = new Q3VBoxLayout(page,0,spacingHint());
	setMainWidget(page);

	list = new KListView(page);
	list->setSelectionMode(Q3ListView::NoSelection);
	list->setItemMargin(4);

	list->addColumn("Episode");
	list->addColumn("Score");
	list->addColumn("Name");

	QString name1,score;
	int h = list->header()->height()+list->itemMargin();
	int minh = 6*h;
	int maxh = 15*h;

	bool any = false;
	EpisodeList *el = EpisodeList::list();
//	QPtrList<Episode> l = EpisodeList::all();
	for (const Episode *e = el->first(); e!=NULL; e = el->next())
	{
		const QString name = e->getName();
		if (name=="---") continue;		// what does this do?
		score = config->readEntry((name+"Score"),"");
		if (score=="0") score = "";
		if (score.isEmpty()) continue;

		name1 = config->readEntry((name+"Name"),"");
		ScoreListItem *i = new ScoreListItem(list,name,score,name1);
		h += i->height();
		any = true;
	}

	if (h<minh) h = minh;
	if (h>maxh) h = maxh;
	list->setFixedSize(list->columnWidth(0)+list->columnWidth(1)+
			   list->columnWidth(2)+(3*2*list->itemMargin()),h);
	vl->addWidget(list);
	vl->addStretch(1);

	if (!any) enableButton(KDialogBase::User1,false);
	adjustSize();
}

void ScoreDialog::slotUser1()
{
	if (KMessageBox::warningContinueCancel(
		    this,"Do you really want to clear the high scores list?",
		    QString::null,KGuiItem("C&lear"))!=KMessageBox::Continue) return;

	config->deleteGroup(groupname);			// deep delete group
	config->sync();

	list->clear();					// nothing now to display
	enableButton(KDialogBase::User1,false);		// can't clear again
}
