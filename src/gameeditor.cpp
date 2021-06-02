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

#include "gameeditor.h"

#include <qlistwidget.h>
#include <qgridlayout.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qstatusbar.h>

#include <kxmlguiwindow.h>
#include <kactioncollection.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <ktoolbar.h>

#include "krepton.h"
#include "mapedit.h"
#include "dataeditor.h"
#include "spriteeditor.h"
#include "mapeditor.h"
#include "newmapdialog.h"
#include "newtransporterdialog.h"
#include "checkmap.h"


static QString formatCoordinates(int x,int y)
{
	if (x<0) return ("");
	return (i18n(" X: %1  Y: %2 ",QString::number(x),QString::number(y)));
}


GameEditor::GameEditor(QWidget *parent)
	: KXmlGuiWindow(parent)
{
	qDebug();

        setObjectName("GameEditor");
        setAttribute(Qt::WA_DeleteOnClose, false);	// we persist once opened

	modified = true;				// force update first time

	sprites = NULL;
	spritewin = NULL;
	mapwin = NULL;

	KStandardAction::close(this, SLOT(close()), actionCollection());

        spriteAction = actionCollection()->addAction("window_spriteeditor", this, SLOT(showSpriteEditor()));
	spriteAction->setText(i18n("Sprite Page"));
	spriteAction->setIcon(QIcon::fromTheme("krepton"));
	actionCollection()->setDefaultShortcut(spriteAction, Qt::Key_F2);

        mapAction = actionCollection()->addAction("window_leveleditor", this, SLOT(showLevelEditor()));
	mapAction->setText(i18n("Map Page"));
	mapAction->setIcon(QIcon::fromTheme("document-encrypted"));
	actionCollection()->setDefaultShortcut(mapAction, Qt::Key_F3);

        dataAction = actionCollection()->addAction("window_dataeditor", this, SLOT(showDataEditor()));
	dataAction->setText(i18n("Episode Page"));
	dataAction->setIcon(QIcon::fromTheme("data-information"));
	actionCollection()->setDefaultShortcut(dataAction, Qt::Key_F4);

        checkAction = actionCollection()->addAction("edit_check", this, SLOT(menuStrictCheck()));
	checkAction->setText(i18n("Check Consistency"));
	checkAction->setIcon(QIcon::fromTheme("dialog-ok-apply"));
	actionCollection()->setDefaultShortcut(checkAction, Qt::Key_F10);

	setupGUI(KXmlGuiWindow::Keys|KXmlGuiWindow::StatusBar|KXmlGuiWindow::Save|KXmlGuiWindow::Create, "kreptonedui.rc");

	QStatusBar *status = statusBar();
	coordsLabel = new QLabel(formatCoordinates(9999,9999));
	coordsLabel->adjustSize();
	coordsLabel->setMinimumWidth(coordsLabel->width());
	status->addPermanentWidget(coordsLabel, 0);

	QWidget *mw = new QWidget(this);
	QGridLayout *l = new QGridLayout(mw);

	tabs = new QTabWidget(mw);
	l->addWidget(tabs,0,0,1,4);

	view = new DataEditor(this);
	dataIndex = tabs->addTab(view, dataAction->icon(), i18n("Episode"));

	spritewin = new SpriteEditor(this);
	connect(spritewin,SIGNAL(changedSprite()),SLOT(changedSprite()));
        connect(spritewin,SIGNAL(coordinatePosition(int,int)),SLOT(slotShowCoordinates(int,int)));
	spriteIndex = tabs->addTab(spritewin, spriteAction->icon(), i18n("Sprites"));

	mapwin = new MapEditor(this);
	connect(mapwin,SIGNAL(modified(bool)),SLOT(setModified(bool)));
        connect(mapwin,SIGNAL(coordinatePosition(int,int)),SLOT(slotShowCoordinates(int,int)));
	mapIndex = tabs->addTab(mapwin, mapAction->icon(), i18n("Map"));

	checkPushButton = new QPushButton(checkAction->icon(), checkAction->text(), mw);
	l->addWidget(checkPushButton,2,1,Qt::AlignCenter);

	closePushButton = new QPushButton(mw);
	KStandardGuiItem::assign(closePushButton, KStandardGuiItem::Close);
	l->addWidget(closePushButton,2,3,Qt::AlignCenter);

	l->setRowStretch(0,1);
	l->setColumnStretch(2,1);

	l->setRowMinimumHeight(1, DialogBase::verticalSpacing());
	l->setColumnMinimumWidth(0, DialogBase::horizontalSpacing());
	l->setColumnMinimumWidth(4, DialogBase::horizontalSpacing());

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

	qDebug() << "done";
}


GameEditor::~GameEditor()
{
	if (sprites!=NULL) delete sprites;
	qDeleteAll(maps);
	maps.clear();

	qDebug() << "done";
}


void GameEditor::updateMapsList()
{
	qDebug();

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
	qDebug() << "level" << level << "item" << item;
	if (level<0) return;
	MapEdit *map = maps.at(level);

	view->transportListBox->clear();
	TransporterList tl = map->getTransportersList();
        for (TransporterList::const_iterator it = tl.constBegin();
		it!=tl.constEnd(); ++it)
	{
		const Transporter *tr = (*it);
		QString s = QString::asprintf("%d,%d -> %d,%d",
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
	qDebug() << item;

	view->removetransportPushButton->setEnabled(item>=0);
	view->changetransportPushButton->setEnabled(item>=0);
	if (mapwin!=NULL) mapwin->selectedTransporter(item);
}



void GameEditor::selectLevel(int level)
{
	qDebug() << "level" << level;

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

		view->removelevelPushButton->setEnabled(maps.count()>0);
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
	qDebug();

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
	qDebug();

//	if (maps.count()==1)				// shouldn't happen, GUI disables
//	{
//		KMessageBox::error(this,"Cannot remove the only level");
//		return;
//	}

	int item = view->mapsListBox->currentRow();
	QString msg = QString("Are you sure to remove the selected map #%1, '%2'?")
		.arg(item+1).arg(view->mapsListBox->currentItem()->text());
	if (KMessageBox::warningContinueCancel(this,msg,QString(),
					       KStandardGuiItem::remove())==KMessageBox::Continue)
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
	qDebug();

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
	qDebug();

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
	qDebug();

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
	qDebug();

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
	qDebug();

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

	if (KMessageBox::warningContinueCancel(this,msg,QString(),
					       KStandardGuiItem::remove())==KMessageBox::Continue)
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
	qDebug();
	if (spritewin!=NULL) spritewin->updateChilds();
	if (mapwin!=NULL) mapwin->updateChilds();
	setModified();
}


void GameEditor::menuStrictCheck()
{
	CheckMap cm(maps);
	cm.report(this,QString(),true);
}


void GameEditor::showSpriteEditor()
{
	tabs->setCurrentIndex(spriteIndex);
}


void GameEditor::showLevelEditor()
{
	int item = view->mapsListBox->currentRow();
	qDebug() << "sel" << item;
	mapwin->setMap(item<0 ? NULL : maps.at(item));

	tabs->setCurrentIndex(mapIndex);
}


void GameEditor::showDataEditor()
{
	tabs->setCurrentIndex(dataIndex);
}


void GameEditor::setModified(bool mod)
{
	qDebug() << "mod?" << mod;

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
	qDebug();

//	saveOptions();
	KXmlGuiWindow::closeEvent(e);
}


void GameEditor::startEdit(const QString name,const MapList ml,const Sprites *ss)
{
	qDebug() << "name" << name;

	epname = name;

	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(ss);

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

	if (spritewin!=NULL) spritewin->setSprites(sprites);
	if (mapwin!=NULL) mapwin->setSprites(sprites);

	qDebug() << "done";
}


void GameEditor::slotShowCoordinates(int x,int y)
{
	coordsLabel->setText(formatCoordinates(x+1,y+1));
}
