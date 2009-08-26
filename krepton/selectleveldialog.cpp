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

#include <klocale.h>

#include <qlistbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qhbox.h>

#include "krepton.h"

#include "gameplayer.h"
#include "pixmaps.h"

#include "selectleveldialog.h"
#include "selectleveldialog.moc"


SelectLevelDialog::SelectLevelDialog(const QStringList &levels,const QString &msg,
                                     QWidget *parent, const char *name)
    : KDialogBase(parent,name,true,QString::null,KDialogBase::Ok|KDialogBase::Cancel)
{
    setCaption("Select Level");

    QVBox *vb = makeVBoxMainWidget();
    vb->setMargin(KDialog::marginHint());
    vb->setSpacing(KDialog::spacingHint());

    (void) new QLabel(msg,vb);
    (void) new QWidget(vb);

    wListBox = new QListBox(vb);
    wListBox->setSelectionMode(QListBox::Single);
    wListBox->setMinimumSize(280,160);
    vb->setStretchFactor(wListBox,1);

    QHBox *hb = new QHBox(vb);
    hb->setSpacing(KDialog::spacingHint());

    QLabel *l = new QLabel(i18n("Password:"),hb);

    wPasswdEdit = new QLineEdit(hb);
    wPasswdEdit->setEchoMode(QLineEdit::Password);
    hb->setStretchFactor(wPasswdEdit,1);
    l->setBuddy(wPasswdEdit);

    mLevelStates.resize(levels.count());

    int toSelect = 0;
    for (QStringList::const_iterator it = levels.begin(); it!=levels.end(); ++it)
    {
        QStringList fields = QStringList::split(" ",(*it));
        GamePlayer::State state = static_cast<GamePlayer::State>(fields[0].toInt());
        int level = fields[1].toInt();

        bool playing = false;
        Pixmaps::type pix;
        switch (state)
        {
case GamePlayer::Unplayed:	pix = (level==0 ? Pixmaps::Unplayed : Pixmaps::Password);	break;
case GamePlayer::Started:	pix = Pixmaps::Started;						break;
case GamePlayer::Playing:	pix = Pixmaps::Playing;	playing = true;				break;
case GamePlayer::Finished:	pix = Pixmaps::Finished;					break;
default:			pix = Pixmaps::Unknown;						break;
        }

        QString pwd = fields[2];
        if (state==GamePlayer::Unplayed && level>0) pwd = QString::null;
        QString txt = QString("  %1: %2").arg(1+level).arg(pwd);

        wListBox->insertItem(Pixmaps::find(pix),txt);
        if (playing) toSelect = wListBox->count()-1;
        else if (toSelect==0 && state==GamePlayer::Started) toSelect = wListBox->count()-1;
        mLevelStates[level] = state;
    }

    wListBox->setSelected(toSelect,true);

    connect(wListBox,SIGNAL(selectionChanged(QListBoxItem *)),SLOT(slotItemSelected(QListBoxItem *)));
    connect(wPasswdEdit,SIGNAL(textChanged(const QString &)),SLOT(slotPasswdChanged()));

    slotItemSelected(wListBox->selectedItem());
}


void SelectLevelDialog::checkButtonOk()
{
    enableButtonOK(wListBox->selectedItem()!=NULL &&
                   (!wPasswdEdit->isEnabled() || wPasswdEdit->text().length()>0));
}


void SelectLevelDialog::slotItemSelected(QListBoxItem *item)
{
    if (item!=NULL)
    {
        int level = item->text().section(':',0,0).toInt()-1;
        //kdDebug() << k_funcinfo << "level " << level << endl;

        bool passwdState = (level>0 && mLevelStates[level]==GamePlayer::Unplayed);
        wPasswdEdit->setEnabled(passwdState);
        if (passwdState) wPasswdEdit->setFocus();
    }
    else wPasswdEdit->setEnabled(false);

    checkButtonOk();
}


void SelectLevelDialog::slotPasswdChanged()
{
    checkButtonOk();
}


QCString SelectLevelDialog::selectedPassword()
{
    QListBoxItem *cur = wListBox->selectedItem();
    if (cur==NULL) return (NULL);
    //kdDebug() << k_funcinfo << "selected [" << cur->text() << "]" << endl;

    QCString pass = cur->text().section(": ",1).local8Bit();
    if (pass.isEmpty()) pass = wPasswdEdit->text().local8Bit();
    //kdDebug() << k_funcinfo << "password[" << pass << "]" << endl;
    return (pass);
}
