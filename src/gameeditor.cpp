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

#include <qgroupbox.h>

#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klistbox.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kmenubar.h>

#ifndef EDITOR_3_WINDOWS
#include <qlayout.h>
#include <qtabwidget.h>
#include <kdialog.h>
#endif

#include "krepton.h"

#include "map2.h"
//#include "editorwidgetui.h"
#include "dataeditor.h"
#include "spriteeditor.h"
#include "mapeditor.h"
#include "newmapdialog.h"
#include "newtransporterdialog.h"
#include "checkmap.h"
#include "parentactionmapper.h"

#include "gameeditor.h"
#include "gameeditor.moc"


GameEditor::GameEditor(KXMLGUIClient *parent,const char *name)
	: KMainWindow(NULL,name,Qt::WType_TopLevel)
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;

	modified = true;				// force update first time
	caption = name;
	align = false;

	sprites = NULL;
	spritewin = NULL;
	mapwin = NULL;

	if (parent!=NULL) (void) new ParentActionMapper(parent,actionCollection());

	toolBar("mainToolBar")->hide();
	menuBar()->hide();

#ifdef EDITOR_3_WINDOWS
	view = new DataEditor(this);
	setCentralWidget(view);
#else
	const int kmh = KDialog::marginHint();
	const int ksh = KDialog::spacingHint();

	QWidget *mw = new QWidget(this);
	QGridLayout *l = new QGridLayout(mw,3,5,kmh,ksh);

	tabs = new QTabWidget(mw);
	l->addMultiCellWidget(tabs,0,0,0,4);

	view = new DataEditor(this);
	tabs->addTab(view,"Episo&de");
	showSpriteEditor(false);
	showLevelEditor(false);

	checkPushButton = new KPushButton("Check Consistenc&y",mw);
	l->addWidget(checkPushButton,2,1,Qt::AlignCenter);
	closePushButton = new KPushButton("&Close",mw);
	l->addWidget(closePushButton,2,3,Qt::AlignCenter);

	l->setRowStretch(0,1);
	l->setColStretch(2,1);

	l->addRowSpacing(1,ksh);
	l->addColSpacing(0,ksh);
	l->addColSpacing(4,ksh);

	setCentralWidget(mw);
	tabs->showPage(view);
#endif
	connect(view->mapsListBox,SIGNAL(selectionChanged()),
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

	connect(view->transportListBox,SIGNAL(selectionChanged()),
		this,SLOT(selectedTransporter()));
	connect(view->newtransportPushButton,SIGNAL(clicked()),
		this,SLOT(transporterInsert()));
	connect(view->changetransportPushButton,SIGNAL(clicked()),
		this,SLOT(transporterChange()));
	connect(view->removetransportPushButton,SIGNAL(clicked()),
		this,SLOT(transporterRemove()));

#ifdef EDITOR_3_WINDOWS
	connect(view->checkPushButton,SIGNAL(clicked()),this,SLOT(menuStrictCheck()));
	connect(view->closePushButton,SIGNAL(clicked()),this,SLOT(close()));
	view->closePushButton->setDefault(true);
#else
	connect(checkPushButton,SIGNAL(clicked()),this,SLOT(menuStrictCheck()));
	connect(closePushButton,SIGNAL(clicked()),this,SLOT(close()));
	closePushButton->setDefault(true);
#endif

	setModified(false);
	updateMapsList();

	kdDebug(0) << k_funcinfo << "done" << endl;
}


GameEditor::~GameEditor()
{
	if (spritewin!=NULL)
	{
		spritewin->hide();
		delete spritewin;
		spritewin = NULL;
	}

	if (mapwin!=NULL)
	{
		mapwin->hide();
		delete mapwin;
		mapwin = NULL;
	}

	if (sprites!=NULL) delete sprites;
	maps.setAutoDelete(true);
	maps.clear();

	kdDebug(0) << k_funcinfo << "done" << endl;
}


void GameEditor::updateMapsList()
{
	kdDebug(0) << k_funcinfo << endl;

	view->mapsListBox->setAutoUpdate(false);
	view->mapsListBox->clear();
	for (const MapEdit *m = maps.first(); m!=NULL; m = maps.next())
		view->mapsListBox->insertItem(m->getPassword());
	view->mapsListBox->setAutoUpdate(true);
}


void GameEditor::updateTransportersList(int item)
{
	int level = view->mapsListBox->index(view->mapsListBox->selectedItem());
	kdDebug(0) << k_funcinfo << "level=" << level << " item=" << item << endl;
	if (level<0) return;
	MapEdit *map = maps.at(level);

	view->transportListBox->setAutoUpdate(false);
	view->transportListBox->clear();

	QPtrList<Transporter> tl = map->getTransportersList();
	for (const Transporter *t = tl.first(); t!=NULL; t = tl.next())
	{
		QString s;
		s.sprintf("%d,%d -> %d,%d",
			  t->orig_x+1,t->orig_y+1,t->dest_x+1,t->dest_y+1);
		view->transportListBox->insertItem(s);
	}

	if (item<-1) item = view->transportListBox->count()-1;
	if (item>=0)
	{
		view->transportListBox->setSelected(item,true);
		view->transportListBox->centerCurrentItem();
	}

	selectedTransporter();
	view->transportListBox->setAutoUpdate(true);
	if (mapwin!=NULL) mapwin->updateChilds();	// if showing transporters
}


void GameEditor::selectedTransporter()
{
	int item = view->transportListBox->index(view->transportListBox->selectedItem());
	kdDebug(0) << k_funcinfo << "item=" << item << endl;

	view->removetransportPushButton->setEnabled(item>=0);
	view->changetransportPushButton->setEnabled(item>=0);
	if (mapwin!=NULL) mapwin->selectedTransporter(item);
}



void GameEditor::selectLevel(int level)
{
	kdDebug(0) << k_funcinfo << "level=" << level << endl;

	if (level<0)					// no selection
	{
		view->passwordLineEdit->setEnabled(false);
		view->timeSpinBox->setEnabled(false);
#ifdef EDITOR_3_WINDOWS
		view->transportGroupBox->setEnabled(false);
#else
		view->transportListBox->setEnabled(false);
		view->newtransportPushButton->setEnabled(false);
		view->changetransportPushButton->setEnabled(false);
		view->removetransportPushButton->setEnabled(false);
#endif
		view->removelevelPushButton->setEnabled(false);
		view->levelupPushButton->setEnabled(false);
		view->leveldownPushButton->setEnabled(false);
		if (mapwin!=NULL)
		{
			mapwin->setMap(NULL);
#ifndef EDITOR_3_WINDOWS
			tabs->setTabEnabled(mapwin,false);
#endif
		}
		return;
	}

	MapEdit *map = maps.at(level);
	if (mapwin!=NULL)
	{
		mapwin->setMap(map);
#ifndef EDITOR_3_WINDOWS
		tabs->setTabEnabled(mapwin,true);
#endif
	}

	view->mapsListBox->blockSignals(true);
	view->passwordLineEdit->blockSignals(true);
	view->timeSpinBox->blockSignals(true);

	view->mapsListBox->setSelected(level,true);
	view->mapsListBox->centerCurrentItem();
	view->passwordLineEdit->setText(map->getPassword());
	view->timeSpinBox->setValue(map->getSeconds());
#ifdef EDITOR_3_WINDOWS
	view->transportGroupBox->setEnabled(true);
#else
	view->transportListBox->setEnabled(true);
	view->newtransportPushButton->setEnabled(true);
#endif
	view->removelevelPushButton->setEnabled(maps.count()>1);
	view->levelupPushButton->setEnabled(level>0);
	view->leveldownPushButton->setEnabled(level<((int) (maps.count())-1));
	view->passwordLineEdit->setEnabled(true);
	view->timeSpinBox->setEnabled(true);

	view->mapsListBox->blockSignals(false);
	view->passwordLineEdit->blockSignals(false);
	view->timeSpinBox->blockSignals(false);

	updateTransportersList();
}


void GameEditor::alignWindows()
{
#ifdef EDITOR_3_WINDOWS
	QRect g = frameGeometry();

	if (spritewin!=NULL)
	{
		spritewin->move(x()+g.width(),y());
		QRect h = spritewin->frameGeometry();
		if (mapwin!=NULL) mapwin->move(x()+g.width(),spritewin->y()+h.height());
	}
	else
	{
		if (mapwin!=NULL) mapwin->move(x()+g.width(),y());
	}
#endif
}


void GameEditor::mapInsert()
{
	kdDebug(0) << k_funcinfo << endl;

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
	view->mapsListBox->setSelected(maps.count()-1,true);
	emit editMapsChange();
}


void GameEditor::mapRemove()
{
	kdDebug(0) << k_funcinfo << endl;

	if (maps.count()==1)				// shouldn't happen, GUI disables
	{
		KMessageBox::error(this,"Cannot remove the only level");
		return;
	}

	int item = view->mapsListBox->index(view->mapsListBox->selectedItem());
	QString msg = QString("Are you sure to remove the selected map #%1, '%2'?")
		.arg(item+1).arg(view->mapsListBox->currentText());
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
	kdDebug(0) << k_funcinfo << endl;

	int item = view->mapsListBox->index(view->mapsListBox->selectedItem());
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
	kdDebug(0) << k_funcinfo << endl;

	int item = view->mapsListBox->index(view->mapsListBox->selectedItem());
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
	kdDebug(0) << k_funcinfo << endl;

	int selm = view->mapsListBox->index(view->mapsListBox->selectedItem());
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
	kdDebug(0) << k_funcinfo << endl;

	int item = view->transportListBox->index(view->transportListBox->selectedItem());
	int selm = view->mapsListBox->index(view->mapsListBox->selectedItem());
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
	kdDebug(0) << k_funcinfo << endl;

	int selm = view->mapsListBox->index(view->mapsListBox->selectedItem());
	if (selm<0) return;				// shouldn't happen, GUI disables
	MapEdit *map = maps.at(selm);

	if (map->getTransportersList().count()<1)
	{						// shouldn't happen, GUI disables
		KMessageBox::error(this,"No transporter to remove");
		return;
	}

	int item = view->transportListBox->index(view->transportListBox->selectedItem());
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
	int level = view->mapsListBox->index(view->mapsListBox->selectedItem());
	if (level>=0) selectLevel(level);
}


void GameEditor::changedPassword(const QString &s)
{
	int level = view->mapsListBox->index(view->mapsListBox->selectedItem());
	if (level<0) return;

	maps.at(level)->changePassword(s);
	setModified();

	view->mapsListBox->setAutoUpdate(false);
	view->mapsListBox->changeItem(s,level);
	view->mapsListBox->setSelected(level,true);
	view->mapsListBox->setAutoUpdate(true);
}


void GameEditor::changedTime(int i)
{
	int level = view->mapsListBox->index(view->mapsListBox->selectedItem());
	if (level<0) return;

	maps.at(level)->changeTime(i);
	setModified();
}


void GameEditor::changedSprite()
{
	kdDebug(0) << k_funcinfo << endl;
	if (spritewin!=NULL) spritewin->updateChilds();
	if (mapwin!=NULL) mapwin->updateChilds();
	setModified();
}


void GameEditor::menuStrictCheck()
{
	CheckMap cm(maps);
	cm.report(this,QString::null,true);
}


void GameEditor::showSpriteEditor(bool show)
{
	kdDebug(0) << k_funcinfo << "spritewin=" <<((void*)spritewin) << endl;

	if (spritewin==NULL)
	{
		spritewin = new SpriteEditor(this,&sprites);
		connect(spritewin,SIGNAL(changedSprite()),
			this,SLOT(changedSprite()));
		connect(spritewin,SIGNAL(closed()),
			this,SLOT(updateWindowStates()));
		spritewin->setEnabled(this->isEnabled());

#ifndef EDITOR_3_WINDOWS
		tabs->addTab(spritewin,"&Sprites");
#endif
	}

#ifdef EDITOR_3_WINDOWS
	if (!show) spritewin->hide();
	else
	{
		spritewin->show();
		if (align) alignWindows();
	}

	updateWindowStates();
#else
	if (show) tabs->showPage(spritewin);
#endif
}


void GameEditor::showLevelEditor(bool show)
{
	kdDebug(0) << k_funcinfo << "mapwin=" <<((void*)mapwin) << endl;

	if (mapwin==NULL)
	{
		mapwin = new MapEditor(this,&sprites);
		connect(mapwin,SIGNAL(modified(bool)),
			this,SLOT(setModified(bool)));
		connect(mapwin,SIGNAL(closed()),
			this,SLOT(updateWindowStates()));
		mapwin->setEnabled(this->isEnabled());

#ifndef EDITOR_3_WINDOWS
		tabs->addTab(mapwin,"&Map");
#endif
	}

	int item = view->mapsListBox->index(view->mapsListBox->selectedItem());
	kdDebug(0) << k_funcinfo << "sel=" << item << endl;
	mapwin->setMap(item<0 ? NULL : maps.at(item));

#ifdef EDITOR_3_WINDOWS
	if (!show) mapwin->hide();
	else
	{
		mapwin->show();
		if (align) alignWindows();
	}

	updateWindowStates();
#else
	if (show) tabs->showPage(mapwin);
#endif
}


void GameEditor::showDataEditor(bool show)
{
#ifndef EDITOR_3_WINDOWS
	if (show) tabs->showPage(view);
#endif
}


void GameEditor::menuRealign()
{
	alignWindows();
}


void GameEditor::moveEvent(QMoveEvent *e)
{
	QWidget::moveEvent(e);
	if (align) alignWindows();
}


void GameEditor::setModified(bool mod)
{
	kdDebug(0) << k_funcinfo << "mod=" << mod << endl;

	if (modified==mod) return;			// no change

	modified = mod;
	updateCaption();
	if (modified) emit editModified();
}


void GameEditor::updateCaption()
{
	setCaption(QString("%1 '%2'").arg(caption).arg(epname),modified);
}


void GameEditor::updateWindowStates()
{
	kdDebug(0) << k_funcinfo << endl;
	emit editWindowChange();
}


void GameEditor::closeEvent(QCloseEvent *e)
{
	kdDebug(0) << k_funcinfo << endl;

	if (spritewin!=NULL) spritewin->hide();
	if (mapwin!=NULL) mapwin->hide();
	hide();

//	saveOptions();
	KMainWindow::closeEvent(e);
	emit closed();
}



void GameEditor::startEdit(const QString name,const MapList ml,const Sprites *ss)
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;

	epname = name;

	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(*ss);

	maps.setAutoDelete(true);
	maps.clear();
	maps.setAutoDelete(false);

	MapListIterator mi(ml);
	for (const Map *mm; (mm = mi.current())!=NULL; ++mi)
	{
		maps.append(new MapEdit(*mm));
	}

//TODO: should this be '0'?
	selectLevel(-1);
	updateMapsList();
	updateCaption();
//	if (doshow) show();

	if (spritewin!=NULL) spritewin->updateChilds();
	if (mapwin!=NULL) mapwin->updateChilds();

	kdDebug(0) << k_funcinfo << "done" << endl;
}


bool GameEditor::spriteVisible() const
{
	return (spritewin!=NULL && spritewin->isVisible());
}

bool GameEditor::mapVisible() const
{
	return (mapwin!=NULL && mapwin->isVisible());
}

void GameEditor::setEnabled(bool enable)
{
	kdDebug(0) << k_funcinfo << endl;

#ifdef EDITOR_3_WINDOWS
	if (spritewin!=NULL) spritewin->setEnabled(enable);
	if (mapwin!=NULL) mapwin->setEnabled(enable);
#endif
	KMainWindow::setEnabled(enable);
}
