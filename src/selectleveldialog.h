////////////////////////////////////////////////////// -*- mode:c++; -*- ///
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

#ifndef SELECTLEVELDIALOG_H
#define SELECTLEVELDIALOG_H


#include <kdialog.h>

#include <q3valuevector.h>
//Added by qt3to4:
#include <Q3CString>

#include "gameplayer.h"


class Q3ListBox;
class Q3ListBoxItem;
class QLineEdit;


class SelectLevelDialog : public KDialog
{
    Q_OBJECT

public:
    SelectLevelDialog(const QStringList &levels, const QString &msg,
                      QWidget *parent = NULL);

    Q3CString selectedPassword();

protected slots:
    void slotItemSelected(Q3ListBoxItem *item);
    void slotPasswdChanged();

private:
    void checkButtonOk();

    Q3ListBox *wListBox;
    QLineEdit *wPasswdEdit;

    Q3ValueVector<GamePlayer::State> mLevelStates;
};


#endif							// SELECTLEVELDIALOG_H