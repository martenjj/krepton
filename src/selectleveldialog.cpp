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

#include "selectleveldialog.h"

#include <qlistwidget.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qtooltip.h>
#include <qcoreapplication.h>

#include "krepton.h"

#include "gameplayer.h"
#include "pixmaps.h"


const QString tableRow(const char *imageKey, const QString &text)
{
	QString res = "<tr><td><img src=\"";
	res += Pixmaps::path(imageKey);
	res += "\"></td><td>";
        res += text;
        res += "</td></tr>";
        return (res);
}


SelectLevelDialog::SelectLevelDialog(const QStringList &levels,const QString &msg,
                                     QWidget *parent)
    : DialogBase(parent)
{
    setObjectName("SelectLevelDialog");
    setWindowTitle(i18n("Select Level"));
    setButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    setModal(true);
    setButtonText(QDialogButtonBox::Ok,i18n("Play"));

    QWidget *mw = new QWidget(this);
    QGridLayout *gl = new QGridLayout(mw);

    QLabel *l = new QLabel(msg,mw);
    gl->addWidget(l,0,0,1,2);

    gl->setRowMinimumHeight(1,DialogBase::verticalSpacing());

    wListBox = new QListWidget(mw);
    wListBox->setSelectionMode(QAbstractItemView::SingleSelection);
    wListBox->setMinimumSize(280,160);
    gl->addWidget(wListBox,2,0,1,2);
    gl->setRowStretch(2,1);

    l = new QLabel(i18n("Password:"),mw);
    gl->addWidget(l,3,0,Qt::AlignRight);

    wPasswdEdit = new QLineEdit(mw);
    wPasswdEdit->setEchoMode(QLineEdit::Password);
    wPasswdEdit->installEventFilter(this);
    gl->addWidget(wPasswdEdit,3,1);
    gl->setColumnStretch(1,1);
    l->setBuddy(wPasswdEdit);

    setMainWidget(mw);

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
        if (state==GamePlayer::Unplayed && level>0) pwd = "";
        QString txt = QString("  %1: %2").arg(1+level).arg(pwd);

        wListBox->addItem(new QListWidgetItem(Pixmaps::find(pix),txt));
        if (playing) toSelect = wListBox->count()-1;
        else if (toSelect<0 && state==GamePlayer::Started) toSelect = wListBox->count()-1;
        mLevelStates[level] = state;
    }

    if (toSelect<0) toSelect = 0;
    wListBox->setCurrentRow(toSelect);

    connect(wListBox,SIGNAL(itemSelectionChanged()),SLOT(slotSelectionChanged()));
    connect(wListBox,SIGNAL(itemDoubleClicked(QListWidgetItem *)),SLOT(accept()));
    connect(wPasswdEdit,SIGNAL(textChanged(const QString &)),SLOT(slotCheckButtonOk()));

    QString tt = "<qt><table border=\"0\" cellspacing=\"4\" cellpadding=\"1\">";
    tt += tableRow("unplayed",i18n("First level, start here"));
    tt += tableRow("playing",i18n("Level that was last played"));
    tt += tableRow("started",i18n("Level that has been started, but not yet completed"));
    tt += tableRow("finished",i18n("Level successfully completed"));
    tt += tableRow("password",i18n("Level not started, a password is needed to play it"));
    tt += "</table>";
    wListBox->setToolTip(tt);

    slotSelectionChanged();
}


void SelectLevelDialog::slotCheckButtonOk()
{
    setButtonEnabled(QDialogButtonBox::Ok, wListBox->selectedItems().count()!=0 &&
                     (!wPasswdEdit->isEnabled() || wPasswdEdit->text().length()>0));
}


void SelectLevelDialog::slotSelectionChanged()
{
    const QListWidgetItem *item = wListBox->currentItem();
    if (item!=NULL)
    {
        int level = item->text().section(':',0,0).toInt()-1;

        bool passwdState = (level>0 && mLevelStates[level]==GamePlayer::Unplayed);
        wPasswdEdit->setEnabled(passwdState);
        if (passwdState) wPasswdEdit->setFocus();
    }
    else wPasswdEdit->setEnabled(false);

    slotCheckButtonOk();
}


QByteArray SelectLevelDialog::selectedPassword()
{
    const QListWidgetItem *item = wListBox->currentItem();
    if (item==NULL) return (NULL);

    QByteArray pass = item->text().section(": ",1).toLocal8Bit();
    if (pass.isEmpty()) pass = wPasswdEdit->text().toLocal8Bit();
    return (pass);
}


bool SelectLevelDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type()==QEvent::KeyPress)
    {
        QKeyEvent *kev = static_cast<QKeyEvent *>(ev);

        const int k = kev->key();
        if (k==Qt::Key_Up || k==Qt::Key_Down)
        {
            // If the list widget has keyboard focus, then the user can
            // move up and down levels using the up/down arrow keys.
            // However, as soon as a level is selected which needs a
            // password, the password edit takes focus (set explicitly
            // in slotSelectionChanged() above) so that the password can
            // be entered.  The arrow keys then no longer move up and
            // down through the list, which is disconcerting if wanting
            // to select another level.
            //
            // Since the up/down arrow keys are not used by QLineEdit,
            // this event filter detects them and sends them to the
            // list widget instead.  This moves its highlight without
            // the list widget taking focus.
            //
            // Must use sendEvent() and not postEvent(), otherwise there
            // is a crash:
            //
            // QCoreApplication::removePostedEvent: Event of type 6 deleted
            //   while posted to QListWidget
            // free(): invalid size

            QCoreApplication::sendEvent(wListBox, ev);
            return (true);				// event handled
        }

    }

    return (false);					// pass the event on
}
