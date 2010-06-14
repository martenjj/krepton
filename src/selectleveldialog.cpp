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

#include <klocale.h>

#include <q3listbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <q3vbox.h>
#include <qlayout.h>
#include <q3hbox.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <Q3CString>

#include "krepton.h"

#include "gameplayer.h"
#include "pixmaps.h"

#include "selectleveldialog.h"
#include "selectleveldialog.moc"


SelectLevelDialog::SelectLevelDialog(const QStringList &levels,const QString &msg,
                                     QWidget *parent)
    : KDialog(parent)
{
    setObjectName("SelectLevelDialog");
    setCaption(i18n("Select Level"));
    setButtons(KDialog::Ok|KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setModal(true);
    showButtonSeparator(true);

    Q3VBox *vb = new Q3VBox(this);
    setMainWidget(vb);
    vb->setMargin(KDialog::marginHint());
    vb->setSpacing(KDialog::spacingHint());

    (void) new QLabel(msg,vb);
    (void) new QWidget(vb);

    wListBox = new Q3ListBox(vb);
    wListBox->setSelectionMode(Q3ListBox::Single);
    wListBox->setMinimumSize(280,160);
    vb->setStretchFactor(wListBox,1);

    Q3HBox *hb = new Q3HBox(vb);
    hb->setSpacing(KDialog::spacingHint());

    QLabel *l = new QLabel(i18n("Password:"),hb);

    wPasswdEdit = new QLineEdit(hb);
    wPasswdEdit->setEchoMode(QLineEdit::Password);
    hb->setStretchFactor(wPasswdEdit,1);
    l->setBuddy(wPasswdEdit);

    mLevelStates.resize(levels.count());

    int toSelect = -1;
    for (QStringList::const_iterator it = levels.begin(); it!=levels.end(); ++it)
    {
        QStringList fields = (*it).split(" ");
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
        else if (toSelect<0 && state==GamePlayer::Started) toSelect = wListBox->count()-1;
        mLevelStates[level] = state;
    }

    if (toSelect<0) toSelect = 0;
    wListBox->setSelected(toSelect,true);

    connect(wListBox,SIGNAL(selectionChanged(Q3ListBoxItem *)),SLOT(slotItemSelected(Q3ListBoxItem *)));
    connect(wListBox,SIGNAL(returnPressed(Q3ListBoxItem *)),SLOT(slotOk()));
    connect(wListBox,SIGNAL(doubleClicked(Q3ListBoxItem *)),SLOT(slotOk()));
    connect(wPasswdEdit,SIGNAL(textChanged(const QString &)),SLOT(slotPasswdChanged()));

    Pixmaps::find(Pixmaps::Finished,true);		// set MIME source, see docs for
    Pixmaps::find(Pixmaps::Password,true);		// QMimeSourceFactory
    Pixmaps::find(Pixmaps::Playing,true);
    Pixmaps::find(Pixmaps::Started,true);
    Pixmaps::find(Pixmaps::Unplayed,true);

    wListBox->setToolTip(i18n("<qt>\
<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\
<tr><td><img src=\"pixmap_unplayed\"></td><td>First level, start here</td></tr>\
<tr><td><img src=\"pixmap_playing\"></td><td>Level that was last played</td></tr>\
<tr><td><img src=\"pixmap_started\"></td><td>A level that has been started, but not yet finished</td></tr>\
<tr><td><img src=\"pixmap_finished\"></td><td>Level successfully completed</td></tr>\
<tr><td><img src=\"pixmap_password\"></td><td>Level not started, a password is needed to play it</td></tr>\
</table>"));

    slotItemSelected(wListBox->selectedItem());
}


void SelectLevelDialog::checkButtonOk()
{
    enableButtonOk(wListBox->selectedItem()!=NULL &&
                   (!wPasswdEdit->isEnabled() || wPasswdEdit->text().length()>0));
}


void SelectLevelDialog::slotItemSelected(Q3ListBoxItem *item)
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


Q3CString SelectLevelDialog::selectedPassword()
{
    Q3ListBoxItem *cur = wListBox->selectedItem();
    if (cur==NULL) return (NULL);

    Q3CString pass = cur->text().section(": ",1).toLocal8Bit();
    if (pass.isEmpty()) pass = wPasswdEdit->text().toLocal8Bit();
    return (pass);
}