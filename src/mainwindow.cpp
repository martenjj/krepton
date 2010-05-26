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

#include <kapplication.h>
#include <kstatusbar.h>
#include <kmainwindow.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kpassdlg.h>
#include <kmessagebox.h>

#include <qdir.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3CString>
#include <QPixmap>
#include <QCloseEvent>

#include "krepton.h"
#include "pixmaps.h"
#include "episodes.h"
#include "sprites.h"
#include "gameplayer.h"
#include "checkmap.h"
#include "selectgamedialog.h"
#include "scoredialog.h"
#include "newscoredialog.h"
#include "gameeditor.h"
#include "saveepisodedialog.h"
#include "selectleveldialog.h"
#include "sounds.h"
#include "importwizard.h"

#include "mainwindow.h"
#include "mainwindow.moc"


static const int minimum_wid = 5;			// sizes for main window
static const int minimum_hei = 3;
static const int default_wid = 9;
static const int default_hei = 9;


MainWindow::MainWindow(QWidget *parent,const char *name)
        : KMainWindow(parent,name)
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;

	currentepisode = NULL;
	modified = false;
        editWarned = false;

	(void) KStdAction::quit(this,SLOT(close()),actionCollection(),"file_quit");

	selectAction = new KAction(i18n("Select Game..."),"fileopen",
				   KStdAccel::shortcut(KStdAccel::Open),
				   this,SLOT(slotSelectGame()),
				   actionCollection(),"file_select");

	startAction = new KAction(i18n("Start at first level"),Qt::Key_S,
					this,SLOT(slotStartGame()),
					actionCollection(),"game_start");

	restartAction = new KAction(i18n("Restart last level"),Qt::Key_R,
					this,SLOT(slotRestartGame()),
					actionCollection(),"game_restart");

	continueAction = new KAction(i18n("Restart at level..."),Qt::Key_C,
					this,SLOT(slotContinueGame()),
					actionCollection(),"game_continue");

	pauseAction = new KToggleAction(i18n("Pause"),"P;Pause",
					this,SLOT(slotPauseGame()),
					actionCollection(),"game_pause");

	suicideAction = new KAction(i18n("Give up"),Qt::Key_Escape,
				this,SLOT(slotSuicide()),
				actionCollection(),"game_suicide");

	(void) new KAction(i18n("High Scores..."),Qt::CTRL+Qt::Key_H,
			   this,SLOT(slotHighScores()),
			   actionCollection(),"file_highscores");

	soundsAction = new KToggleAction(i18n("Sounds"),0,
					 this,SLOT(slotSoundsChanged()),
					 actionCollection(),"settings_sounds");

	loadspritesAction = new KAction("&Change Sprites...",0,
					this,SLOT(slotLoadSprites()),
					actionCollection(),"settings_loadsprites");

	magnificationList = new KSelectAction(i18n("Display Size"),0,
					      this,SLOT(slotSetMagnification()),
					      actionCollection(),"settings_magnification");

	editAction = new KAction(i18n("Game Editor..."),Qt::CTRL+Qt::Key_E,
				   this,SLOT(slotEdit()),
				   actionCollection(),"edit_edit");

	saveAction = KStdAction::save(this,SLOT(slotSave()),
				      actionCollection());

	saveAsAction = KStdAction::saveAs(this,SLOT(slotSaveAs()),
				      actionCollection());

	importAction = new KAction(i18n("Import..."),0,
				   this,SLOT(slotImport()),
				   actionCollection(),"file_import");

	exportAction = new KAction(i18n("Export..."),0,
				   this,SLOT(slotExport()),
				   actionCollection(),"file_export");

	printAction = new KAction(i18n("Print..."),"fileprint",
				  KStdAccel::shortcut(KStdAccel::Print),
				  this,SLOT(slotPrint()),
				  actionCollection(),"file_print");

	removeAction = new KAction(i18n("Remove..."),0,
				   this,SLOT(slotRemove()),
				   actionCollection(),"file_remove");

	checkAction = new KAction(i18n("Check Consistency"),"ok",Qt::Key_F10,
				  this,SLOT(slotStrictCheck()),
				  actionCollection(),"edit_check");

	strictToggle = new KToggleAction(i18n("Check Consistency Before Saving"),0,
					 actionCollection(),"settings_check");
#ifdef EDITOR_3_WINDOWS
	alignToggle = new KToggleAction(i18n("Align Editor Windows"),0,
					 this,SLOT(slotAlignChanged()),
					 actionCollection(),"settings_align");

	spriteToggle = new KToggleAction(i18n("Sprite Editor"),Qt::Key_F2,
					 this,SLOT(slotSpriteEditor()),
					 actionCollection(),"window_spriteeditor");

	mapToggle = new KToggleAction(i18n("Map Editor"),Qt::Key_F3,
				      this,SLOT(slotLevelEditor()),
				      actionCollection(),"window_leveleditor");

	realignAction = new KAction(i18n("Realign Windows"),Qt::Key_F4,
				    this,SLOT(slotRealignEditor()),
				    actionCollection(),"window_realign");
#else
	spriteAction = new KAction(i18n("Sprite Page"),Qt::Key_F2,
				   this,SLOT(slotSpriteEditor()),
				   actionCollection(),"window_spriteeditor");

	mapAction = new KAction(i18n("Map Page"),Qt::Key_F3,
				this,SLOT(slotLevelEditor()),
				actionCollection(),"window_leveleditor");

	dataAction = new KAction(i18n("Episode Page"),Qt::Key_F4,
				 this,SLOT(slotDataEditor()),
				 actionCollection(),"window_dataeditor");
#endif
	createGUI("kreptonui.rc");

	QStringList list;
	list << i18n("Half") << i18n("Normal") << i18n("Double");
	magnificationList->setItems(list);

	KStatusBar *status = statusBar();
	status->insertFixedItem(i18n("Diamonds: Plenty!"),1);
	status->insertFixedItem(i18n("Time: 999:99"),2);
	status->insertFixedItem(i18n("Score: 999999"),3);

	const QPixmap keypix = Pixmaps::find(Pixmaps::Key);
	keyflag = new QLabel(this,QString::null,Qt::WStyle_Customize|Qt::WStyle_NoBorder);
	keyflag->setFixedSize(keypix.size());
	keyflag->setPixmap(keypix);
	keyflag->setEnabled(false);

	const QPixmap crownpix = Pixmaps::find(Pixmaps::Crown);
	crownflag = new QLabel(this,QString::null,Qt::WStyle_Customize|Qt::WStyle_NoBorder);
	crownflag->setFixedSize(crownpix.size());
	crownflag->setPixmap(crownpix);
	crownflag->setEnabled(false);

	const QPixmap livespix = Pixmaps::findLives(0);
	livesflag = new QLabel(this,QString::null,Qt::WStyle_Customize|Qt::WStyle_NoBorder);
	livesflag->setFixedSize(livespix.size());
	livesflag->setPixmap(livespix);
	livesflag->setEnabled(false);

	status->addWidget(keyflag,0,true);
	status->addWidget(crownflag,0,true);
	status->addWidget(livesflag,0,true);

	for (int i = 1; i<=3; ++i) status->setItemAlignment(i,Qt::AlignLeft|Qt::AlignVCenter);
	updateStats(-1);

	game = new GamePlayer(this);
	setCentralWidget(game);
	edit = NULL;

	connect(game, SIGNAL(changedStats(int,int,int)),
		this, SLOT(updateStats(int,int,int)));
	connect(game, SIGNAL(changedFlags(bool,bool)),
		this, SLOT(updateFlags(bool,bool)));
	connect(game, SIGNAL(changedLives(int)),
		this, SLOT(updateLives(int)));

	connect(game,SIGNAL(changedPlayState(bool,bool)),this,SLOT(updatePlayState(bool,bool)));
	connect(game,SIGNAL(changedGameState(bool)),this,SLOT(updateGameState(bool)));
	connect(game,SIGNAL(gameOver()),this,SLOT(gameOver()));

	toolBar("mainToolBar")->hide();

	readOptions();

	QSize s1(minimum_wid*Sprites::sprite_width,minimum_hei*Sprites::sprite_height);
	setBaseSize(sizeForCentralWidgetSize(s1));
	setSizeIncrement(Sprites::sprite_width,Sprites::sprite_height);
	QSize s2(default_wid*Sprites::sprite_width,default_hei*Sprites::sprite_height);
	resize(sizeForCentralWidgetSize(s2));
	magnificationList->setCurrentItem((int) Sprites::getMagnification());

	setAutoSaveSettings();
	finalizeGUI(false);

	updateGameState(false);
	updateWindowStates();

	kdDebug(0) << k_funcinfo << "done" << endl;
}


MainWindow::~MainWindow()
{
	if (game!=NULL) delete game;
	if (edit!=NULL) delete edit;
}


void MainWindow::readOptions()
{
	kdDebug(0) << k_funcinfo << endl;

	KConfig *config = KGlobal::config();

	config->setGroup("Options");
	soundsAction->setChecked(config->readBoolEntry("Sounds",true));
	int mag = config->readNumEntry("Magnification",Sprites::Normal);

	config->setGroup("Editor");
	strictToggle->setChecked(config->readBoolEntry("StrictChecking",true));
#ifdef EDITOR_3_WINDOWS
	alignToggle->setChecked(config->readBoolEntry("AutoAlign",true));
#endif
	Sprites::setMagnification((Sprites::Magnification) mag);
	slotSoundsChanged();
	slotAlignChanged();
}


void MainWindow::saveOptions()
{
	kdDebug(0) << k_funcinfo << endl;

	KConfig *config = KGlobal::config();
	config->setGroup("Options");
	config->writeEntry("Sounds",soundsAction->isChecked());

	config->setGroup("Editor");
	config->writeEntry("StrictChecking",strictToggle->isChecked());
#ifdef EDITOR_3_WINDOWS
	config->writeEntry("AutoAlign",alignToggle->isChecked());
#endif
}


bool MainWindow::queryClose()
{
	kdDebug(0) << k_funcinfo << "mod=" << modified << endl;

	if (currentepisode==NULL) return (true);	// no episode loaded
	const bool emod = (edit==NULL ? false : edit->isModified());
	if (!modified && !emod) return (true);		// can close now

	switch (KMessageBox::warningYesNoCancel(this,
                                                (currentepisode->isGlobal() ?
                                                 i18n("<qt>Save a copy of episode <b>%1</b>?") :
                                                 i18n("<qt>Save changes to episode <b>%1</b>?")).arg(currentepisode->getName())))
	{
case KMessageBox::Yes:
		slotSave();				// save the episode
		return (!modified);			// check it succeeded

case KMessageBox::No:
		return (true);				// discard

default:	return (false);				// cancel
	}
}


void MainWindow::updateCaption()
{
	QString cap = i18n("Game");

	if (currentepisode)
	{
		cap += QString(" '%1'").arg(currentepisode->getName());
		if (game->inGame()) cap += i18n(" level %1").arg(game->lastLevel()+1);
	}
	setCaption(cap,modified);
}


void MainWindow::slotHighScores()
{
	ScoreDialog d(this);
	d.exec();
}


void MainWindow::slotSoundsChanged()
{
	Sound::setEnabled(soundsAction->isChecked());
}


void MainWindow::slotAlignChanged()
{
#ifdef EDITOR_3_WINDOWS
	if (edit!=NULL) edit->setAlign(alignToggle->isChecked());
#endif
}


void MainWindow::updateStats(int diamonds,int secs,int points)
{
	KStatusBar *status = statusBar();

	QString ds = QString::null;
	if (diamonds>=0)
	{
		if (diamonds==0) ds = i18n("Finish!");
		else if (diamonds>50) ds = i18n("Plenty!");
		else ds.setNum(diamonds);
		ds = i18n("Diamonds: %1").arg(ds);
	}
	status->changeItem(ds,1);

	QString ts = QString::null;
	if (secs>=0)
	{
		const int min = secs / 60;
		const int sec = secs % 60;
		ts.sprintf(i18n("Time: %3d:%02d"),min,sec);
	}
	status->changeItem(ts,2);

	status->changeItem((points<0 ? QString::null : i18n("Score: %1").arg(points)),3);
}


void MainWindow::updateFlags(bool key,bool crown)
{
	keyflag->setEnabled(key);
	crownflag->setEnabled(crown);
}


void MainWindow::updateLives(int lives)
{
	kdDebug(0) << k_funcinfo << " lives=" << lives << endl;
	livesflag->setPixmap(Pixmaps::findLives(lives));
}


void MainWindow::updateGameState(bool playing)
{
	kdDebug(0) << k_funcinfo << "playing=" << playing << endl;

	const bool loaded = (currentepisode!=NULL);
	selectAction->setEnabled(!playing && EpisodeList::list()->any());
	loadspritesAction->setEnabled(loaded);
	saveAction->setEnabled(loaded && !playing && !currentepisode->isGlobal());

	editAction->setEnabled(loaded && !playing);
	saveAsAction->setEnabled(loaded && !playing);
	importAction->setEnabled(!playing);
	exportAction->setEnabled(loaded && !playing);
	printAction->setEnabled(loaded && !playing);
	removeAction->setEnabled(EpisodeList::list()->anyUser());

	livesflag->setEnabled(playing);

	if (edit!=NULL) edit->setEnabled(!playing);
	if (!playing) updatePlayState(false,false);

	updateCaption();
}


void MainWindow::updatePlayState(bool ingame,bool inpause)
{
	kdDebug(0) << k_funcinfo << "game=" << ingame << " pause=" << inpause << endl;

	const bool loaded = (currentepisode!=NULL);

	pauseAction->setChecked(inpause);
	pauseAction->setEnabled(ingame);
	suicideAction->setEnabled(ingame);

	startAction->setEnabled(loaded && !ingame && game->countLevels()>0);
	restartAction->setEnabled(loaded && !ingame && game->lastLevel()>=0);
	continueAction->setEnabled(loaded && !ingame);
}


void MainWindow::gameOver()
{
	updateFlags(false,false);
	updateStats(-1);
	updateLives(0);

	KConfig *config = KGlobal::config();
	config->setGroup("High Score");

	QString name = currentepisode->getName();
	QString score = config->readEntry((name+"Score"),"0");
	if (game->getPoints()>score.toInt())
	{
		NewScoreDialog d(this);
		if (d.exec())
		{
			score.setNum(game->getPoints());
			config->writeEntry((name+"Score"),score);
			config->writeEntry((name+"Name"),d.name());

			ScoreDialog h(this);
			h.exec();
		}
	}
}


void MainWindow::updateEditModified()
{
	kdDebug(0) << k_funcinfo << endl;

	modified = true;
	updateCaption();
}


void MainWindow::slotStartGame()
{
	kdDebug(0) << k_funcinfo << endl;
	if (currentepisode==NULL) return;		// not loaded, shouldn't happen

	fetchFromEditor();
	game->startGame(currentepisode);
}


void MainWindow::slotRestartGame()
{
	kdDebug(0) << k_funcinfo << endl;
	if (currentepisode==NULL) return;		// not loaded, shouldn't happen

	fetchFromEditor();
	game->startGame(currentepisode,game->lastLevel());
}


void MainWindow::slotContinueGame()
{
	kdDebug(0) << k_funcinfo << endl;
	if (currentepisode==NULL) return;		// not loaded, shouldn't happen
	fetchFromEditor();

	QStringList levels = game->listLevels(currentepisode);

	Q3CString pwd = NULL;
	QString msg = i18n("<qt>Continuing episode <b>%1</b>").arg(currentepisode->getName());
	if (levels.count()>0)
	{
		SelectLevelDialog d(levels,msg,this);
		if (!d.exec()) return;

		pwd = d.selectedPassword();
	}

	if (pwd.isEmpty())
	{
		if (KPasswordDialog::getPassword(pwd,msg)!=KPasswordDialog::Accepted) return;
	}

	pwd = pwd.stripWhiteSpace();
	if (pwd.isEmpty()) KMessageBox::information(this,i18n("No idea? You need to start at the first level."));
	else game->startGamePassword(currentepisode,pwd);
}


void MainWindow::slotLoadSprites()
{
	if (!queryClose()) return;

	SelectGameDialog d(i18n("Select Sprites"),this);
	if (!d.exec()) return;
	const Episode *e = d.selectedItem();
	kdDebug(0) << k_funcinfo << "selected = '" << e->getName() << "'" << endl;

	const QString status = prepareGame(e,true);
	if (!status.isNull())
	{
		KMessageBox::error(
			this,i18n("<qt>Unable to load the sprites for episode <b>%1</b>."
                                  "<br>%2"
                                  "<p>The application may not be properly installed.")
			.arg(e->getName())
			.arg(status));
		return;
	}

	game->repaint(false);
	if (edit!=NULL) passToEditor();
}


void MainWindow::slotPauseGame()
{
	kdDebug(0) << k_funcinfo << endl;
	game->pauseAction();
}


void MainWindow::slotSuicide()
{
	kdDebug(0) << k_funcinfo << endl;
	game->suicideAction();
}


void MainWindow::slotSelectGame()
{
	kdDebug(0) << k_funcinfo << endl;
	if (!queryClose()) return;

	SelectGameDialog d(i18n("Select Game"),this);
	if (!d.exec()) return;
	const Episode *e = d.selectedItem();
	kdDebug(0) << k_funcinfo << "selected = '" << e->getName() << "'" << endl;

	loadGame(e);
	if (edit!=NULL) passToEditor();
}


void MainWindow::loadGame(const Episode *e)		// from GUI selection
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

	const QString status = prepareGame(e);
	if (!status.isNull())
	{
		KMessageBox::error(
			this,i18n("<qt>Unable to load the episode <b>%1</b>."
                                  "<br>%2"
                                  "<p>The application may not be properly installed.")
			.arg(e->getName())
			.arg(status));
		return;
	}

	if (game->countLevels()==0)
	{
		KMessageBox::information(
			this,i18n("<qt>The episode <b>%1</b> has no levels available to play."
                                  "<p>You can use its sprites with another episode, or"
                                  "<br>create new levels in the editor.").arg(e->getName()),
			QString::null,"emptyMapsMessage");
	}

	modified = false;
        editWarned = false;
	updateGameState(false);
}


void MainWindow::loadGame(const QString name)		// from command line
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;
	const Episode *e = EpisodeList::list()->find(name);
	if (e==NULL)
	{
		KMessageBox::error(
			this,i18n("<qt>Unable to load the episode named <b>%1</b>."
                                  "<p>There may be no episode with that name, or the application may not be properly installed.").arg(name));
		return;
	}

	loadGame(e);
}


void MainWindow::slotSetMagnification()
{
        const int selection = magnificationList->currentItem();
	kdDebug(0) << k_funcinfo << " selected=" << selection << endl;
	if (selection==((int) Sprites::getMagnification())) return;

	KMessageBox::information(this,i18n("This change will take effect when %1 is next started.").arg(kapp->caption()),
				 QString::null,"sizeChangeMessage");

	KConfig *config = KGlobal::config();
	config->setGroup("Options");
	config->writeEntry("Magnification",selection);
}


const QString MainWindow::prepareGame(const Episode *e,bool spritesonly)
{
	if (e==NULL) return (QString::null);
	kdDebug(0) << k_funcinfo << "name=" << e->getName() << endl;

	if (spritesonly)
        {
            editWarned = false;
            return (game->loadSprites(e));
        }

	const QString status = game->loadEpisode(e);
	currentepisode = (status.isNull() ? e : NULL);
	return (status);
}


void MainWindow::slotEdit()
{
	kdDebug(0) << k_funcinfo << "edit=" << ((void*)edit) << endl;

	if (currentepisode==NULL) return;

        if (game->getSprites()->hasMultiLevels() && !editWarned)
        {
            switch (KMessageBox::questionYesNoCancel(this,
                                                     i18n("<qt><p>\
This game contains level-specific sprite files.  If you edit the sprites using \
the sprite editor, you will only be editing the default sprites which are used for any \
levels that do not have specific sprite images.  For levels that do have \
specific sprites, changes made in the sprite editor will have no effect.\
<p>\
To remove the level-specific sprite files, select <b>Remove</b>.  All levels in \
the game will then use the same sprite images;  these can be edited using the sprite \
editor in the usual way.  No files will be removed from the saved copy of the \
game until it is saved.\
<p>\
To retain the level-specific sprite files, select <b>Keep</b>.  Changes made \
using the sprite editor may not have any effect on played levels."),\
                                                     i18n("Multiple Sprites"),
                                                     KGuiItem("Remove"),
                                                     KGuiItem("Keep")))
            {
case KMessageBox::Yes:
		game->getSprites()->removeMultiLevels();
                break;

case KMessageBox::No:
                break;

case KMessageBox::Cancel:
		return;
            }

            editWarned = true;				// not again for this game
        }

	if (edit==NULL)
	{
		edit = new GameEditor(this,i18n("Editor"));
		connect(edit,SIGNAL(editWindowChange()),
			this,SLOT(updateWindowStates()));
		connect(edit,SIGNAL(closed()),
			this,SLOT(updateWindowStates()));
		connect(edit,SIGNAL(editModified()),
			this,SLOT(updateEditModified()));
		connect(edit,SIGNAL(editMapsChange()),
			this,SLOT(updateEditLevels()));

		slotAlignChanged();
	}

	passToEditor();
	edit->show();
	updateWindowStates();
}


void MainWindow::closeEvent(QCloseEvent *e)
{
	kdDebug(0) << k_funcinfo << endl;

	if (edit!=NULL) edit->close();
	saveOptions();
	KMainWindow::closeEvent(e);
}


void MainWindow::fetchFromEditor()
{
	if (edit==NULL) return;
	kdDebug(0) << k_funcinfo << "mod=" << edit->isModified() << endl;

	if (!edit->isModified()) return;		// nothing to do
	if (game->inGame()) return;			// shouldn't happen

	setCursor(QCursor(Qt::WaitCursor));
	game->setSprites(edit->getSprites());
	game->setMaps(edit->getMaps());
	unsetCursor();

	modified = true;
	updateGameState(false);
	edit->setModified(false);
}


void MainWindow::passToEditor()
{
	kdDebug(0) << k_funcinfo << endl;

	setCursor(QCursor(Qt::WaitCursor));
	edit->startEdit(currentepisode->getName(),game->getMaps(),game->getSprites());
	unsetCursor();
}


bool MainWindow::prepareSave()
{
	fetchFromEditor();
	if (strictToggle->isChecked())
	{
		CheckMap cm(game->getMaps());
		if (cm.status()!=CheckMap::Ok)
		{
			if (cm.report(this,i18n("Do you want to save this episode?"),
				      false)==KMessageBox::No) return (false);
		}
	}

	return (true);
}


void MainWindow::slotSave()
{
	if (!prepareSave()) return;
	if (currentepisode==NULL) return;		// shouldn't happen
	if (currentepisode->isGlobal())			// global, save somewhere else
	{
		slotSaveAs();
		return;
	}

	if (game->saveEpisode(currentepisode)!=NULL)	// user, save there
	{
		modified = false;
		updateGameState(false);
	}
}


void MainWindow::slotSaveAs()
{
	if (!prepareSave()) return;

	SaveEpisodeDialog d(i18n("Save Episode"),this);
	if (!d.exec()) return;

	kdDebug(0) << k_funcinfo << "name='" << d.name() << "' path='" << d.path() << "'" << endl;

	QDir dir(d.path());
	if (dir.exists())
	{
		if (KMessageBox::warningContinueCancel(
			    this,i18n("<qt>Overwrite existing episode <b>%1</b>?").arg(d.name()),
			    QString::null,
			    KGuiItem(i18n("Overwrite")))!=KMessageBox::Continue) return;
	}

	const Episode *e = game->saveEpisode(d.name(),d.path());
	if (e!=NULL)
	{
		currentepisode = e;
		modified = false;
		updateGameState(false);
	}
}


void MainWindow::slotRemove()
{
	kdDebug(0) << k_funcinfo << endl;

	SelectGameDialog d(i18n("Remove Game"),this,NULL,true);
	if (!d.exec()) return;

	const Episode *e = d.selectedItem();
	if (e->isGlobal()) return;			// shouldn't happen

	const QString name = e->getName();
	kdDebug(0) << k_funcinfo << "selected = '" << e->getName() << "'" << endl;

	if (KMessageBox::warningContinueCancel(
		    this,i18n("<qt>Are you sure you want to remove the episode <b>%2</b>,"
                              "<br>located at '%1'?")
		    .arg(Episode::savePath(name)).arg(name),
		    QString::null,KGuiItem(i18n("Remove")))!=KMessageBox::Continue) return;

	kdDebug(0) << "e=" << ((void*)e) << " current=" << ((void*)currentepisode) << endl;

	const bool iscurrent = (currentepisode!=NULL && e==currentepisode);
							// is it current episode?
	if (!e->removeFiles()) return;			// remove episode files
	EpisodeList::list()->remove(e,iscurrent);	// remove from our list
	updateGameState(false);				// may have been the last

	if (iscurrent)					// have removed the current?
	{
		modified = true;			// yes, so now it's unsaved
		updateCaption();
	}
}


void MainWindow::slotImport()
{
	kdDebug(0) << k_funcinfo << endl;
        ImportWizard wiz(i18n("Import Wizard"),this);
        if (wiz.exec())
	{
		QString newEpisodeName = wiz.newEpisodeToLoad();
		if (newEpisodeName.isNull()) return;
		if (!queryClose()) return;
		loadGame(newEpisodeName);
	}
}


void MainWindow::slotExport()
{
	kdDebug(0) << k_funcinfo << endl;
	KMessageBox::sorry(this,i18n("This option is not implemented yet.\nSee the TODO file for more information."));
}


void MainWindow::slotPrint()
{
	kdDebug(0) << k_funcinfo << endl;
	KMessageBox::sorry(this,i18n("This option is not implemented yet.\nSee the TODO file for more information."));
}


void MainWindow::updateWindowStates()
{
	kdDebug(0) << k_funcinfo << endl;

	const bool evis = (edit==NULL ? false : edit->isVisible());
#ifdef EDITOR_3_WINDOWS
	const bool svis = (edit==NULL ? false : edit->spriteVisible());
	const bool mvis = (edit==NULL ? false : edit->mapVisible());

	spriteToggle->setChecked(svis);
	spriteToggle->setEnabled(evis);
	mapToggle->setChecked(mvis);
	mapToggle->setEnabled(evis);
	realignAction->setEnabled(evis && (svis || mvis));
#else
	dataAction->setEnabled(evis);
	mapAction->setEnabled(evis);
	spriteAction->setEnabled(evis);
#endif
	checkAction->setEnabled(evis);
}


void MainWindow::slotStrictCheck()
{
	if (edit!=NULL) edit->menuStrictCheck();
}


void MainWindow::slotSpriteEditor()
{
#ifdef EDITOR_3_WINDOWS
	bool show = spriteToggle->isChecked();
#else
	bool show = true;
#endif
	if (edit!=NULL) edit->showSpriteEditor(show);
}


void MainWindow::slotLevelEditor()
{
#ifdef EDITOR_3_WINDOWS
	bool show = mapToggle->isChecked();
#else
	bool show = true;
#endif
	if (edit!=NULL) edit->showLevelEditor(show);
}


void MainWindow::slotDataEditor()
{
#ifndef EDITOR_3_WINDOWS
	if (edit!=NULL) edit->showDataEditor(true);
#endif
}


void MainWindow::slotRealignEditor()
{
	if (edit!=NULL) edit->menuRealign();
}


void MainWindow::updateEditLevels()
{
	kdDebug(0) << k_funcinfo << endl;

	fetchFromEditor();				// need to update 'lastlevel'
	updatePlayState(false,false);
}
