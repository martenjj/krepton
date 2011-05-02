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

#include <qlistwidget.h>
#include <qgridlayout.h>
#include <qtabwidget.h>

#include <kxmlguiwindow.h>
#include <kactioncollection.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kstatusbar.h>

#include "krepton.h"

#include "map2.h"
#include "dataeditor.h"
#include "spriteeditor.h"
#include "mapeditor.h"
#include "newmapdialog.h"
#include "newtransporterdialog.h"
#include "checkmap.h"

#include "gameeditor.h"
#include "gameeditor.moc"


GameEditor::GameEditor(QWidget *parent)
	: KXmlGuiWindow(parent)
{
	kDebug();

        setObjectName("GameEditor");
        setAttribute(Qt::WA_DeleteOnClose, false);	// we persist once opened

	modified = true;				// force update first time

	sprites = NULL;
	spritewin = NULL;
	mapwin = NULL;

	KStandardAction::close(this, SLOT(close()), actionCollection());

        spriteAction = new KAction(i18n("Sprite Page"), this);
        spriteAction->setShortcut(Qt::Key_F2);
        connect(spriteAction, SIGNAL(triggered()), SLOT(showSpriteEditor()));
        actionCollection()->addAction("window_spriteeditor", spriteAction);

        mapAction = new KAction(i18n("Map Page"), this);
        mapAction->setShortcut(Qt::Key_F3);
        connect(mapAction, SIGNAL(triggered()), SLOT(showLevelEditor()));
        actionCollection()->addAction("window_leveleditor", mapAction);

        dataAction = new KAction(i18n("Episode Page"), this);
        dataAction->setShortcut(Qt::Key_F4);
        connect(dataAction, SIGNAL(triggered()), SLOT(showDataEditor()));
        actionCollection()->addAction("window_dataeditor", dataAction);

        checkAction = new KAction(i18n("Check Consistency"), this);
        checkAction->setShortcut(Qt::Key_F10);
        connect(checkAction, SIGNAL(triggered()), SLOT(menuStrictCheck()));
        actionCollection()->addAction("edit_check", checkAction);

	setupGUI(KXmlGuiWindow::Keys|KXmlGuiWindow::StatusBar|KXmlGuiWindow::Save|KXmlGuiWindow::Create, "kreptonedui.rc");

	KStatusBar *status = statusBar();
	status->insertPermanentFixedItem(formatCoordinates(9999,9999), 1);

	QWidget *mw = new QWidget(this);
	QGridLayout *l = new QGridLayout(mw);

	tabs = new QTabWidget(mw);
	l->addWidget(tabs,0,0,1,4);

	view = new DataEditor(this);
	dataIndex = tabs->addTab(view,"Episode");

	spritewin = new SpriteEditor(this,&sprites);
	connect(spritewin,SIGNAL(changedSprite()),SLOT(changedSprite()));
        connect(spritewin,SIGNAL(coordinatePosition(int,int)),SLOT(slotShowCoordinates(int,int)));
	spriteIndex = tabs->addTab(spritewin,"Sprites");

	mapwin = new MapEditor(this,&sprites);
	connect(mapwin,SIGNAL(modified(bool)),SLOT(setModified(bool)));
        connect(mapwin,SIGNAL(coordinatePosition(int,int)),SLOT(slotShowCoordinates(int,int)));
	mapIndex = tabs->addTab(mapwin,"Map");

	checkPushButton = new KPushButton("Check Consistency",mw);
	l->addWidget(checkPushButton,2,1,Qt::AlignCenter);

	closePushButton = new KPushButton(KStandardGuiItem::close(),mw);
	l->addWidget(closePushButton,2,3,Qt::AlignCenter);

	l->setRowStretch(0,1);
	l->setColumnStretch(2,1);

	l->setRowMinimumHeight(1, KDialog::spacingHint());
	l->setColumnMinimumWidth(0, KDialog::spacingHint());
	l->setColumnMinimumWidth(4, KDialog::spacingHint());

	setCentralWidget(mw);
	tabs->setCurrentIndex(dataIndex);

	connect(view->mapsListBox,SIGNAL(itemSelectionChanged()),
		this,SLOT(selectedMap()));
	connect(view->passwordLineEdit,SIGNAL(textChanged(const QString&)),
		this,SLOT(changedPassword(const QString&)));
	connect(view->timeSpinBox,SIGNAL(valueChanged(int)),
		this,SLOT(changedTime(int)));

	connect(view->newlevelPushButton,SIGNAL(clicked()),
		this,SLOT(mapInsert()));
	connect(view->removelevelPushButton,SIGNAL(clicked()),
		this,SLOT(mapRemove()));
	connect(view->levelupPushButton,SIGNAL(clicked()),
		this,SLOT(mapMoveUp()));
	connect(view->leveldownPushButton,SIGNAL(clicked()),
		this,SLOT(mapMoveDown()));

	connect(view->transportListBox,SIGNAL(itemSelectionChanged()),
		this,SLOT(selectedTransporter()));
	connect(view->newtransportPushButton,SIGNAL(clicked()),
		this,SLOT(transporterInsert()));
	connect(view->changetransportPushButton,SIGNAL(clicked()),
		this,SLOT(transporterChange()));
	connect(view->removetransportPushButton,SIGNAL(clicked()),
		this,SLOT(transporterRemove()));

	connect(checkPushButton,SIGNAL(clicked()),this,SLOT(menuStrictCheck()));
	connect(closePushButton,SIGNAL(clicked()),this,SLOT(close()));
	closePushButton->setDefault(true);

	setAutoSaveSettings();
	finalizeGUI(false);
	toolBar("editToolBar")->hide();

	setModified(false);
	updateMapsList();
        slotShowCoordinates(-1,-1);

	kDebug() << "done";
}


GameEditor::~GameEditor()
{
	if (sprites!=NULL) delete sprites;
	qDeleteAll(maps);
	maps.clear();

	kDebug() << "done";
}


void GameEditor::updateMapsList()
{
	kDebug();

	view->mapsListBox->clear();
	for (MapEditList::const_iterator it = maps.constBegin();
		it!=maps.constEnd(); ++it)
	{
		const MapEdit *m = (*it);
		view->mapsListBox->addItem(m->getPassword());
        }
}


void GameEditor::updateTransportersList(int item)
{
	int level = view->mapsListBox->currentRow();
	kDebug() << "level=" << level << " item=" << item;
	if (level<0) return;
	MapEdit *map = maps.at(level);

	view->transportListBox->clear();
	TransporterList tl = map->getTransportersList();
        for (TransporterList::const_iterator it = tl.constBegin();
		it!=tl.constEnd(); ++it)
	{
		const Transporter *tr = (*it);
		QString s;
		s.sprintf("%d,%d -> %d,%d",
			  tr->orig_x+1,tr->orig_y+1,tr->dest_x+1,tr->dest_y+1);
		view->transportListBox->addItem(s);
	}

	if (item<-1) item = view->transportListBox->count()-1;
	if (item>=0)
	{
		view->transportListBox->setCurrentRow(item);
                view->transportListBox->scrollToItem(view->transportListBox->currentItem());
	}

	selectedTransporter();
	if (mapwin!=NULL) mapwin->updateChilds();	// if showing transporters
}


void GameEditor::selectedTransporter()
{
	int item = view->transportListBox->currentRow();
	kDebug() << "item=" << item;

	view->removetransportPushButton->setEnabled(item>=0);
	view->changetransportPushButton->setEnabled(item>=0);
	if (mapwin!=NULL) mapwin->selectedTransporter(item);
}



void GameEditor::selectLevel(int level)
{
	kDebug() << "level=" << level;

	if (level<0)					// no selection
	{
		view->passwordLineEdit->setEnabled(false);
		view->timeSpinBox->setEnabled(false);
		view->transportListBox->setEnabled(false);
		view->newtransportPushButton->setEnabled(false);
		view->changetransportPushButton->setEnabled(false);
		view->removetransportPushButton->setEnabled(false);

		view->removelevelPushButton->setEnabled(false);
		view->levelupPushButton->setEnabled(false);
		view->leveldownPushButton->setEnabled(false);
		if (mapwin!=NULL)
		{
			mapwin->setMap(NULL);
			tabs->setTabEnabled(mapIndex, false);
		}
	}
        else
        {
		MapEdit *map = maps.at(level);
		if (mapwin!=NULL)
		{
			mapwin->setMap(map);
			tabs->setTabEnabled(mapIndex, true);
		}

		view->mapsListBox->blockSignals(true);
		view->passwordLineEdit->blockSignals(true);
		view->timeSpinBox->blockSignals(true);

		view->mapsListBox->setCurrentRow(level);
	        view->mapsListBox->scrollToItem(view->mapsListBox->currentItem());
		view->passwordLineEdit->setText(map->getPassword());
		view->timeSpinBox->setValue(map->getSeconds());
		view->transportListBox->setEnabled(true);
		view->newtransportPushButton->setEnabled(true);

		view->removelevelPushButton->setEnabled(maps.count()>1);
		view->levelupPushButton->setEnabled(level>0);
		view->leveldownPushButton->setEnabled(level<static_cast<int>(maps.count()-1));
		view->passwordLineEdit->setEnabled(true);
		view->timeSpinBox->setEnabled(true);

		view->mapsListBox->blockSignals(false);
		view->passwordLineEdit->blockSignals(false);
		view->timeSpinBox->blockSignals(false);

		updateTransportersList();
        }

	mapAction->setEnabled(tabs->isTabEnabled(mapIndex));
}


void GameEditor::mapInsert()
{
	kDebug();

	NewMapDialog d(this);

	int sx,sy;
	for (;;)
	{
		if (!d.exec()) return;
		if (d.mapPassword()=="")		// shouldn't happen, GUI disables
		{
			KMessageBox::error(this,"Name/password not specified");
			continue;
		}

		sx = d.mapWidth();
		sy = d.mapHeight();
		if (sx<10 || sy<10)			// shouldn't happen, spinboxes enforce
		{
			KMessageBox::error(this,"Size is too small");
			continue;
		}
		break;
	}

	maps.mapInsert(sx,sy,d.mapPassword());
	setModified();
//TODO: need 'selectLevel' here?
	updateMapsList();
	view->mapsListBox->setCurrentRow(maps.count()-1);
	emit editMapsChange();
}


void GameEditor::mapRemove()
{
	kDebug();

	if (maps.count()==1)				// shouldn't happen, GUI disables
	{
		KMessageBox::error(this,"Cannot remove the only level");
		return;
	}

	int item = view->mapsListBox->currentRow();
	QString msg = QString("Are you sure to remove the selected map #%1, '%2'?")
		.arg(item+1).arg(view->mapsListBox->currentItem()->text());
	if (KMessageBox::warningContinueCancel(this,msg,QString::null,
					       KGuiItem("&Remove"))==KMessageBox::Continue)
	{
		maps.mapRemove(item);
		setModified();

//TODO: should this be '0'?
		selectLevel(-1);
		updateMapsList();
		emit editMapsChange();
	}
}


void GameEditor::mapMoveUp()
{
	kDebug();

	int item = view->mapsListBox->currentRow();
	if (item<=0)					// shouldn't happen, GUI disables
	{
		KMessageBox::error(this,"Cannot move up the first level");
		return;
	}

	maps.mapMoveUp(item);
	setModified();

	updateMapsList();
	selectLevel(item-1);
}


void GameEditor::mapMoveDown()
{
	kDebug();

	int item = view->mapsListBox->currentRow();
	int last_item = maps.count()-1;
	if (item>=last_item)				// shouldn't happen, GUI disables
	{
		KMessageBox::error(this,"Cannot move down the last level");
		return;
	}

	maps.mapMoveDown(item);
	setModified();

	updateMapsList();
	selectLevel(item+1);
}


void GameEditor::transporterInsert()
{
	kDebug();

	int selm = view->mapsListBox->currentRow();
	if (selm<0) return;				// shouldn't happen, GUI disables
	MapEdit *map = maps.at(selm);

	NewTransporterDialog d("New Transporter",this);

	d.setLimits(map->getWidth(),map->getHeight());
	d.setValues(1,1,2,2);
	if (!d.exec()) return;

	map->transporterInsert(d.xorig(),d.yorig(),d.xdest(),d.ydest());
	setModified();

	updateTransportersList(-2);
}


void GameEditor::transporterChange()
{
	kDebug();

	int item = view->transportListBox->currentRow();
	int selm = view->mapsListBox->currentRow();
	if (selm<0) return;				// shouldn't happen, GUI disables
	MapEdit *map = maps.at(selm);

	int ox,oy,dx,dy;
	map->transporterGet(item,&ox,&oy,&dx,&dy);

	NewTransporterDialog d("Change Transporter",this);
	d.setLimits(map->getWidth(),map->getHeight());
	d.setValues(ox,oy,dx,dy);
	if (!d.exec()) return;

	map->transporterChange(item,d.xorig(),d.yorig(),d.xdest(),d.ydest());
	setModified();

	updateTransportersList(item);
}


void GameEditor::transporterRemove()
{
	kDebug();

	int selm = view->mapsListBox->currentRow();
	if (selm<0) return;				// shouldn't happen, GUI disables
	MapEdit *map = maps.at(selm);

	if (map->getTransportersList().count()<1)
	{						// shouldn't happen, GUI disables
		KMessageBox::error(this,"No transporter to remove");
		return;
	}

	int item = view->transportListBox->currentRow();
	QString msg = QString("Are you sure to remove the selected transporter #%1?").arg(item+1);

	if (KMessageBox::warningContinueCancel(this,msg,QString::null,
					       KGuiItem("&Remove"))==KMessageBox::Continue)
	{
		map->transporterRemove(item);
		setModified();

		updateTransportersList();
	}
}


void GameEditor::selectedMap()
{
	if (view->mapsListBox->selectedItems().count()==0) return;
	int level = view->mapsListBox->currentRow();
	if (level>=0) selectLevel(level);
}


void GameEditor::changedPassword(const QString &s)
{
	int level = view->mapsListBox->currentRow();
	if (level<0) return;

	maps.at(level)->changePassword(s.toLocal8Bit());
	setModified();

	view->mapsListBox->item(level)->setText(s);
	view->mapsListBox->setCurrentRow(level);
}


void GameEditor::changedTime(int i)
{
	int level = view->mapsListBox->currentRow();
	if (level<0) return;

	maps.at(level)->changeTime(i);
	setModified();
}


void GameEditor::changedSprite()
{
	kDebug();
	if (spritewin!=NULL) spritewin->updateChilds();
	if (mapwin!=NULL) mapwin->updateChilds();
	setModified();
}


void GameEditor::menuStrictCheck()
{
	CheckMap cm(maps);
	cm.report(this,QString::null,true);
}


void GameEditor::showSpriteEditor()
{
	tabs->setCurrentIndex(spriteIndex);
}


void GameEditor::showLevelEditor()
{
	int item = view->mapsListBox->currentRow();
	kDebug() << "sel=" << item;
	mapwin->setMap(item<0 ? NULL : maps.at(item));

	tabs->setCurrentIndex(mapIndex);
}


void GameEditor::showDataEditor()
{
	tabs->setCurrentIndex(dataIndex);
}


void GameEditor::setModified(bool mod)
{
	kDebug() << "mod=" << mod;

	if (modified==mod) return;			// no change

	modified = mod;
	updateCaption();
	if (modified) emit editModified();
}


void GameEditor::updateCaption()
{
	setCaption(i18n("Editor '%1'", epname), modified);
}


void GameEditor::closeEvent(QCloseEvent *e)
{
	kDebug();

//	saveOptions();
	KXmlGuiWindow::closeEvent(e);
}


void GameEditor::startEdit(const QString name,const MapList ml,const Sprites *ss)
{
	kDebug() << "name='" << name << "'";

	epname = name;

	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(*ss);

        qDeleteAll(maps);
	maps.clear();

        for (MapList::const_iterator mi = ml.constBegin();
		mi!=ml.constEnd(); ++mi)
	{
		const Map *mm = (*mi);
		maps.append(new MapEdit(*mm));
	}

//TODO: should this be '0'?
	selectLevel(-1);
	updateMapsList();
	updateCaption();

	if (spritewin!=NULL) spritewin->updateChilds();
	if (mapwin!=NULL) mapwin->updateChilds();

	kDebug() << "done";
}


void GameEditor::slotShowCoordinates(int x,int y)
{
	KStatusBar *status = statusBar();
	status->changeItem(formatCoordinates(x,y), 1);
}


QString GameEditor::formatCoordinates(int x,int y)
{
	if (x<0) return (QString::null);
	return (i18n(" X: %1  Y: %2 ",QString::number(x),QString::number(y)));
}
