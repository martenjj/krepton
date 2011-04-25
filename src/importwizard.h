/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//  
//  KRepton - the classic Repton game for KDE
//  
//  Copyright (c) 2008 Jonathan Marten <jjm@keelhaul.demon.co.uk>
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

#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H

#include <k3wizard.h>

#include "importmanager.h"


class QListWidget;
class QLineEdit;
class QLabel;
class QCheckBox;

class KUrlRequester;
class KTextEdit;


class ImportWizard : public K3Wizard
{
	Q_OBJECT

public:
	ImportWizard(const QString &title,QWidget *parent);

	QString newEpisodeToLoad() const;

protected:
	void showPage(QWidget *page);
	void next();

protected slots:
	void slotPage1FormatSelected();
	void slotPage2SourceSelected();
	void slotPage3NameChanged();

private:
	const ImportManager::formatInfo *format;
	const ImportManager *manager;
	QString episodeName;
	bool importedOk;

	void setupPage1();
	QWidget *page1;
	QListWidget *page1list;
	QLabel *page1info;

	void setupPage2();
	QWidget *page2;
	QLabel *page2info;
	KUrlRequester *page2source;

	void setupPage3();
	QWidget *page3;
	QLineEdit *page3name;
	QCheckBox *page3over;

	void setupPage4();
	QWidget *page4;
	KTextEdit *page4disp;

	void setupPage5();
	QWidget *page5;
	KTextEdit *page5disp;
	QCheckBox *page5load;
};

#endif							// IMPORTWIZARD_H
