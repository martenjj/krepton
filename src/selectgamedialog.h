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

#ifndef SELECTGAMEDIALOG_H
#define SELECTGAMEDIALOG_H

#include <kfdialog/dialogbase.h>

class QListWidget;
class QListWidgetItem;
class Episode;
class EpisodeList;


class SelectGameDialog : public DialogBase
{
    Q_OBJECT

public:
    SelectGameDialog(const QString title, QWidget *parent = NULL, bool useronly = false);
    ~SelectGameDialog();

    const Episode *selectedItem();

public slots:
    int exec() override;

protected slots:
    void slotSelectionChanged();
    void slotExecuted(QListWidgetItem *item);
    void timerTick();

private:
    QListWidget *wEpisodeIconView;

    QTimer *icontimer;
    int nexticon;
    EpisodeList *episodes;
};


#endif							// !SELECTGAMEDIALOG_H
