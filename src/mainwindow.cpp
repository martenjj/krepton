////////////////////////// -*- indent-tabs-mode:t; c-basic-offset:8; -*- ///
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

#include "krepton.h"
#include "mainwindow.h"

#include <kxmlguiwindow.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include <kselectaction.h>
#include <ktoggleaction.h>
#include <kpassworddialog.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <ktoolbar.h>

#ifdef HAVE_LIBKDEGAMES
#include <kstandardgameaction.h>
#include <highscore/kscoredialog.h>
#endif

#include <qdir.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qapplication.h>

#include "pixmaps.h"
#include "episodes.h"
#include "sprites.h"
#include "sounds.h"
#include "gameplayer.h"
#include "checkmap.h"
#include "selectgamedialog.h"
#include "gameeditor.h"
#include "saveepisodedialog.h"
#include "selectleveldialog.h"
#include "sounds.h"
#include "importwizard.h"
#include "cheatdialog.h"
#include "settings.h"


static const int minimum_wid = 5;			// sizes for main window
static const int minimum_hei = 3;
static const int default_wid = 9;
static const int default_hei = 9;


MainWindow::MainWindow(QWidget *parent)
        : KXmlGuiWindow(parent)
{
	qDebug();

	currentepisode = NULL;
	modified = false;
        editWarned = false;
	cheats_used = Cheat::NoCheats;

	KStandardAction::quit(this, SLOT(close()), actionCollection());

        selectAction = KStandardAction::open(this, SLOT(slotSelectGame()), actionCollection());
        selectAction->setText(i18n("Select Game..."));

	// TODO: possibly combine "Start at first level" and "Restart at level"
#ifdef HAVE_LIBKDEGAMES
	startAction = KStandardGameAction::gameNew(this, SLOT(slotStartGame()), actionCollection());
#else
	startAction = actionCollection()->addAction("game_new", this, SLOT(slotStartGame()));
        actionCollection()->addAction("game_start", startAction);
#endif
        actionCollection()->setDefaultShortcut(startAction, Qt::Key_S);
        startAction->setText(i18n("Start at first level"));
	startAction->setIcon(QIcon::fromTheme("go-next"));

#ifdef HAVE_LIBKDEGAMES
	restartAction = KStandardGameAction::restart(this, SLOT(slotRestartGame()), actionCollection());
#else
	restartAction = actionCollection()->addAction("game_restart", this, SLOT(slotRestartGame()));
	restartAction->setIcon(QIcon::fromTheme("view-refresh"));
        actionCollection()->addAction("game_restart", restartAction);
#endif
        actionCollection()->setDefaultShortcut(restartAction, Qt::Key_R);
        restartAction->setText(i18n("Restart last level"));

        continueAction= actionCollection()->addAction("game_continue", this, SLOT(slotContinueGame()));
	continueAction->setText(i18n("Restart at level..."));
	continueAction->setIcon(QIcon::fromTheme("go-last"));
        actionCollection()->setDefaultShortcut(continueAction, Qt::Key_C);

#ifdef HAVE_LIBKDEGAMES
	pauseAction = KStandardGameAction::pause(this, SLOT(slotPauseGame()), actionCollection());
#else
	pauseAction = actionCollection()->addAction("game_pause", this, SLOT(slotPauseGame()));
	pauseAction->setIcon(QIcon::fromTheme("media-playback-pause"));
        actionCollection()->setDefaultShortcuts(pauseAction, (QList<QKeySequence>() << Qt::Key_P << Qt::Key_Pause));
#endif

#ifdef HAVE_LIBKDEGAMES
	suicideAction = KStandardGameAction::end(this, SLOT(slotSuicide()), actionCollection());
#else
	suicideAction = actionCollection()->addAction("game_end", this, SLOT(slotSuicide()));
	suicideAction->setIcon(QIcon::fromTheme("window-close"));
#endif
        actionCollection()->setDefaultShortcut(suicideAction, Qt::Key_Escape);
        suicideAction->setText(i18n("Give up"));

#ifdef HAVE_LIBKDEGAMES
	QAction *scoresAction = KStandardGameAction::highscores(this, SLOT(slotHighScores()), actionCollection());
#else
	QAction *scoresAction = actionCollection()->addAction("file_highscores", this, SLOT(slotHighScores()));
        scoresAction->setIcon(QIcon::fromTheme("games-highscores"));
        actionCollection()->setDefaultShortcut(scoresAction, Qt::CTRL+Qt::Key_H);
#endif
	scoresAction->setText(i18n("High Scores..."));

	KConfigSkeletonItem *ski = Settings::self()->soundEnabledItem();
        soundsEnableAction = new KToggleAction(ski->label(), this);
	soundsEnableAction->setChecked(Settings::soundEnabled());
        connect(soundsEnableAction, SIGNAL(triggered()), SLOT(slotSoundsEnable()));
        actionCollection()->addAction("settings_sound_enable", soundsEnableAction);

	ski = Settings::self()->soundSchemeItem();
	soundsSchemeList = new KSelectAction(ski->label(), this);
	soundsSchemeList->setIcon(QIcon::fromTheme("audio-volume-medium"));
        connect(soundsSchemeList, SIGNAL(triggered(QAction *)), SLOT(slotSoundsScheme(QAction *)));
        actionCollection()->addAction("settings_sound_scheme", soundsSchemeList);

	QMap<QString,QString> schemeMap = Sound::self()->allSchemesList();
	const QString curScheme = Sound::self()->schemeName();
	for (QMap<QString,QString>::const_iterator it = schemeMap.constBegin();
	     it!=schemeMap.constEnd(); ++it)
	{
		QString scheme = it.key();
		QAction *act = soundsSchemeList->addAction(it.value());
		if (scheme==curScheme) act->setChecked(true);
		act->setData(scheme);
	}

        loadspritesAction= actionCollection()->addAction("settings_loadsprites", this, SLOT(slotLoadSprites()));
	loadspritesAction->setText(i18n("Change Sprites..."));
	loadspritesAction->setIcon(QIcon::fromTheme("krepton"));

	ski = Settings::self()->magnificationItem();
	magnificationList = new KSelectAction(ski->label(), this);
	magnificationList->setIcon(QIcon::fromTheme("zoom-in"));
        connect(magnificationList, SIGNAL(triggered(int)), SLOT(slotSetMagnification(int)));
        actionCollection()->addAction("settings_magnification", magnificationList);

        cheatsAction = actionCollection()->addAction("settings_cheats", this, SLOT(slotSelectCheats()));
	cheatsAction->setText(i18n("Cheat Modes..."));
	cheatsAction->setIcon(QIcon::fromTheme("dialog-warning"));

        editAction = actionCollection()->addAction("edit_edit", this, SLOT(slotEdit()));
	editAction->setText(i18n("Game Editor..."));
        editAction->setIcon(QIcon::fromTheme("document-edit"));
	actionCollection()->setDefaultShortcut(editAction, Qt::CTRL+Qt::Key_E);

	saveAction = KStandardAction::save(this, SLOT(slotSave()), actionCollection());
	saveAsAction = KStandardAction::saveAs(this, SLOT(slotSaveAs()), actionCollection());

        importAction = actionCollection()->addAction("file_import", this, SLOT(slotImport()));
        importAction->setIcon(QIcon::fromTheme("document-import"));
	importAction->setText(i18n("Import..."));

        exportAction = actionCollection()->addAction("file_export", this, SLOT(slotExport()));
        exportAction->setIcon(QIcon::fromTheme("document-export"));
	exportAction->setText(i18n("Export..."));

        printAction = KStandardAction::print(this, SLOT(slotPrint()), actionCollection());

        removeAction = actionCollection()->addAction(KStandardAction::DeleteFile, "file_remove", this, SLOT(slotRemove()));
	removeAction->setText(i18n("Remove..."));

	ski = Settings::self()->strictCheckingEnabledItem();
        strictToggle = new KToggleAction(ski->label(), this);
	strictToggle->setChecked(Settings::strictCheckingEnabled());
	connect(strictToggle, &KToggleAction::toggled, this, [](bool on) { Settings::setStrictCheckingEnabled(on); });
        actionCollection()->addAction("settings_check", strictToggle);

	ski = Settings::self()->flashForTimeLimitItem();
        flashToggle = new KToggleAction(ski->label(), this);
	flashToggle->setChecked(Settings::flashForTimeLimit());
	connect(flashToggle, &KToggleAction::toggled, this, [](bool on) { Settings::setFlashForTimeLimit(on); });
        actionCollection()->addAction("settings_flash", flashToggle);

	setupGUI(KXmlGuiWindow::Keys|KXmlGuiWindow::StatusBar|KXmlGuiWindow::Save|KXmlGuiWindow::Create, "kreptonui.rc");

	QStringList list;
	list << i18n("Half") << i18n("Normal") << i18n("Double");
	magnificationList->setItems(list);

	QStatusBar *status = statusBar();

	diamondsCount = new QLabel(i18n("Diamonds: Plenty!  "));
	diamondsCount->adjustSize();
	diamondsCount->setMinimumWidth(diamondsCount->width());
	timeDisplay = new QLabel(i18n("Time: 999:99  "));
	timeDisplay->adjustSize();
	timeDisplay->setMinimumWidth(timeDisplay->width());
	scoreDisplay = new QLabel(i18n("Score: 999999  "));
	scoreDisplay->adjustSize();
	scoreDisplay->setMinimumWidth(scoreDisplay->width());

	const QPixmap keypix = Pixmaps::find(Pixmaps::Key);
	keyflag = new QLabel(this);
	keyflag->setFixedSize(keypix.size());
	keyflag->setPixmap(keypix);
	keyflag->setEnabled(false);

	const QPixmap crownpix = Pixmaps::find(Pixmaps::Crown);
	crownflag = new QLabel(this);
	crownflag->setFixedSize(crownpix.size());
	crownflag->setPixmap(crownpix);
	crownflag->setEnabled(false);

	const QPixmap livespix = Pixmaps::findLives(0);
	livesflag = new QLabel(this);
	livesflag->setFixedSize(livespix.size());
	livesflag->setPixmap(livespix);
	livesflag->setEnabled(false);

	// addPermanentWidget() adds the new item at the extreme right of the
	// status bar, so the order here is left to right as seen by the user.
	status->addPermanentWidget(livesflag, 0);
	status->addPermanentWidget(keyflag, 0);
	status->addPermanentWidget(crownflag, 0);
	status->addPermanentWidget(diamondsCount, 0);
	status->addPermanentWidget(timeDisplay, 0);
	status->addPermanentWidget(scoreDisplay, 0);

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

	readOptions();
	if (cheats_used!=Cheat::NoCheats) game->setCheats(cheats_used);

	QSize s1(minimum_wid*Sprites::sprite_width,minimum_hei*Sprites::sprite_height);
        // TODO: what's the equivalent?
	//setBaseSize(sizeForCentralWidgetSize(s1));
	setSizeIncrement(Sprites::sprite_width,Sprites::sprite_height);
	QSize s2(default_wid*Sprites::sprite_width,default_hei*Sprites::sprite_height);
	//resize(sizeForCentralWidgetSize(s2));
	magnificationList->setCurrentItem(static_cast<int>(Sprites::getMagnification()));

	setAutoSaveSettings();
	finalizeGUI(false);
	toolBar("mainToolBar")->hide();

	updateGameState(false);

	qDebug() << "done";
}


void MainWindow::readOptions()
{
	qDebug();

	const QString soundScheme = Settings::soundScheme();
	if (!soundScheme.isEmpty()) Sound::self()->setSchemeName(soundScheme);
	slotSoundsEnable();
	updateSoundsMenu();

	const int mag = Settings::magnification();
	Sprites::setMagnification(static_cast<Sprites::Magnification>(mag));
}


void MainWindow::saveOptions()
{
	qDebug();

	Settings::setSoundEnabled(soundsEnableAction->isChecked());
	Settings::setSoundScheme(Sound::self()->schemeConfigName());
	Settings::self()->save();
}


bool MainWindow::queryClose()
{
	qDebug() << "mod=" << modified;

	if (currentepisode==NULL) return (true);	// no episode loaded
	const bool emod = (edit==NULL ? false : edit->isModified());
	if (!modified && !emod) return (true);		// can close now

	switch (KMessageBox::warningYesNoCancel(this,
                                                (currentepisode->isGlobal() ?
                                                 i18n("<qt>Save a copy of episode <b>%1</b>?", currentepisode->getName()) :
                                                 i18n("<qt>Save changes to episode <b>%1</b>?", currentepisode->getName()))))
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
		if (game->inGame()) cap += i18n(" level %1", game->lastLevel()+1);
	}
	setCaption(cap,modified);
}


#ifdef HAVE_LIBKDEGAMES
static KScoreDialog *prepareScoreDialog(QWidget *parent)
{
	auto *d = new KScoreDialog(KScoreDialog::Name|KScoreDialog::Level|KScoreDialog::Score|KScoreDialog::Date|KScoreDialog::Custom1, parent);
	d->setAttribute(Qt::WA_DeleteOnClose, true);
	d->addField(KScoreDialog::Custom1, i18n("Episode"), "episode");
	return (d);
}
#endif


#ifdef HAVE_LIBKDEGAMES
void MainWindow::slotHighScores()
{
	KScoreDialog *d = prepareScoreDialog(this);
	d->exec();
}
#endif


void MainWindow::slotSoundsEnable()
{
	Sound::self()->setEnabled(soundsEnableAction->isChecked());
}


void MainWindow::slotSoundsScheme(QAction *act)
{
	QString name = act->data().toString();
	qDebug() << "name" << name;
	Sound::self()->setSchemeName(name);
}


void MainWindow::updateSoundsMenu()
{
	QList<QAction *> acts = soundsSchemeList->actions();
	QString curScheme = Sound::self()->schemeName();

	for (QList<QAction *>::const_iterator it = acts.constBegin();
	     it!=acts.constEnd(); ++it)
	{
		QAction *act = (*it);
		act->setChecked(act->data()==curScheme);
	}
}


void MainWindow::updateStats(int diamonds,int secs,int points)
{
	QString ds = "";
	if (diamonds>=0)
	{
		if (diamonds==0) ds = i18n("Finish!");
		else if (diamonds>50) ds = i18n("Plenty!");
		else ds.setNum(diamonds);
		ds = i18n("Diamonds: %1", ds);
	}
	diamondsCount->setText(ds);

	QString ts = "";
	if (secs>=0)
	{
		const int min = secs / 60;
		const int sec = secs % 60;
                ts = i18n("Time: %1:%2", min, QString("%1").arg(sec, 2, 10, QChar('0')));
	}
	timeDisplay->setText(ts);

	scoreDisplay->setText(points<0 ? "" : i18n("Score: %1", points));
}


void MainWindow::updateFlags(bool key,bool crown)
{
	keyflag->setEnabled(key);
	crownflag->setEnabled(crown);
}


void MainWindow::updateLives(int lives)
{
	qDebug() << " lives=" << lives;
	livesflag->setPixmap(Pixmaps::findLives(lives));
}


void MainWindow::updateGameState(bool playing)
{
	qDebug() << "playing=" << playing;

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
	cheatsAction->setEnabled(loaded);

	livesflag->setEnabled(playing);

	if (edit!=NULL) edit->setEnabled(!playing);
	if (!playing) updatePlayState(false,false);

	updateCaption();
}


void MainWindow::updatePlayState(bool ingame,bool inpause)
{
	qDebug() << "game=" << ingame << " pause=" << inpause;

	const bool loaded = (currentepisode!=NULL);

	pauseAction->setChecked(inpause);
	pauseAction->setEnabled(ingame);
	suicideAction->setEnabled(ingame);

	startAction->setEnabled(loaded && !ingame && game->countLevels()>0);
	restartAction->setEnabled(loaded && !ingame && game->lastLevel()>=0);
	continueAction->setEnabled(loaded && !ingame && game->countLevels()>0);
}


void MainWindow::gameOver()
{
	updateFlags(false,false);
	updateStats(-1);
	updateLives(0);

#ifdef HAVE_LIBKDEGAMES
	const int points = game->getPoints();
	if (points>2000)				// no high score for trivial game
	{
		if (!game->everCheated())		// no high score for cheats
		{
			KScoreDialog *d = prepareScoreDialog(this);

			KScoreDialog::FieldInfo scoreInfo;
			scoreInfo[KScoreDialog::Score].setNum(points);
			scoreInfo[KScoreDialog::Level].setNum(game->lastLevel()+1);
			scoreInfo[KScoreDialog::Date] = QLocale::system().toString(QDate::currentDate(), QLocale::ShortFormat);
			scoreInfo[KScoreDialog::Custom1] = currentepisode->getName().toUpper();

			d->addScore(scoreInfo);
			d->exec();
		}
		else qDebug() << "no high score, cheated";
	}
	else qDebug() << "no high score, only" << points << "points";
#endif
}


void MainWindow::updateEditModified()
{
	qDebug();

	modified = true;
	updateCaption();
}


void MainWindow::slotStartGame()
{
	qDebug();
	if (currentepisode==NULL) return;		// not loaded, shouldn't happen

	fetchFromEditor();
	game->startGame(currentepisode);
}


void MainWindow::slotRestartGame()
{
	qDebug();
	if (currentepisode==NULL) return;		// not loaded, shouldn't happen

	fetchFromEditor();
	game->startGame(currentepisode,game->lastLevel());
}


void MainWindow::slotContinueGame()
{
	qDebug();
	if (currentepisode==NULL) return;		// not loaded, shouldn't happen
	fetchFromEditor();

	QStringList levels = game->listLevels(currentepisode);

	QByteArray pwd;
	QString msg = i18n("<qt>Continuing episode <b>%1</b>", currentepisode->getName());
	if (levels.count()>0)
	{
		SelectLevelDialog d(levels,msg,this);
		if (!d.exec()) return;

		pwd = d.selectedPassword();
	}

	if (pwd.isEmpty())
	{
            KPasswordDialog dlg(this);
            dlg.setPrompt(msg);
            if (!dlg.exec()) return;
            pwd = dlg.password().toLocal8Bit();
	}

	pwd = pwd.trimmed();
	if (pwd.isEmpty()) KMessageBox::information(this,i18n("No idea? You need to start at the first level."));
	else game->startGamePassword(currentepisode,pwd);
}


void MainWindow::slotLoadSprites()
{
	if (!queryClose()) return;

	SelectGameDialog d(i18n("Select Sprites"),this);
	if (!d.exec()) return;
	const Episode *e = d.selectedItem();
	qDebug() << "selected = '" << e->getName() << "'";

	const QString status = prepareGame(e,true);
	if (!status.isEmpty())
	{
		KMessageBox::error(
			this,i18n("<qt>Unable to load the sprites for episode <b>%1</b>."
                                  "<br>%2"
                                  "<p>The application may not be properly installed.",
                                  e->getName(), status));
		return;
	}

	game->repaint();
	if (edit!=NULL) passToEditor();
}


void MainWindow::slotPauseGame()
{
	qDebug();
	game->pauseAction();
}


void MainWindow::slotSuicide()
{
	qDebug();
	game->suicideAction();
}


void MainWindow::slotSelectGame()
{
	qDebug();
	if (!queryClose()) return;

	SelectGameDialog d(i18n("Select Game"),this);
	if (!d.exec()) return;
	const Episode *e = d.selectedItem();
	qDebug() << "selected = '" << e->getName() << "'";

	loadGame(e);
	if (edit!=NULL) passToEditor();
}


void MainWindow::loadGame(const Episode *e)		// from GUI selection
{
	qDebug() << "name='" << e->getName() << "'";

	const QString status = prepareGame(e);
	if (!status.isEmpty())
	{
		KMessageBox::error(
			this,i18n("<qt>Unable to load the episode <b>%1</b>."
                                  "<br>%2"
                                  "<p>The application may not be properly installed.",
                                  e->getName(),
                                  status));
		return;
	}

	if (game->countLevels()==0)
	{
		KMessageBox::information(
			this,i18n("<qt>The episode <b>%1</b> has no levels available to play."
                                  "<p>You can use its sprites with another episode, or"
                                  "<br>create new levels in the editor.", e->getName()),
			QString(),"emptyMapsMessage");
	}

	modified = false;
        editWarned = false;
	updateGameState(false);
}


void MainWindow::loadGame(const QString name)		// from command line
{
	qDebug() << "name='" << name << "'";
	const Episode *e = EpisodeList::list()->find(name);
	if (e==NULL)
	{
		KMessageBox::error(
			this,i18n("<qt>Unable to load the episode named <b>%1</b>."
                                  "<p>There may be no episode with that name, or the application may not be properly installed.",
                                  name));
		return;
	}

	loadGame(e);
}


void MainWindow::slotSetMagnification(int idx)
{
	const Sprites::Magnification selection = static_cast<Sprites::Magnification>(idx);
	qDebug() << selection;
	if (selection==Sprites::getMagnification()) return;
	Settings::setMagnification(idx);

	KMessageBox::information(this,
                                 i18n("This change will take effect when %1 is next started.",
                                      qApp->applicationDisplayName()),
				 QString(),"sizeChangeMessage");
}


const QString MainWindow::prepareGame(const Episode *e,bool spritesonly)
{
	if (e==NULL) return ("");
	qDebug() << "name=" << e->getName();

	if (spritesonly)
        {
            editWarned = false;
            return (game->loadSprites(e));
        }

	const QString status = game->loadEpisode(e);
	currentepisode = (status.isEmpty() ? e : NULL);
	return (status);
}


void MainWindow::slotEdit()
{
	qDebug() << "edit=" << edit;

	if (currentepisode==NULL) return;

        if (game->getSprites()->hasMultiLevels() && !editWarned)
        {
		KMessageBox::ButtonCode q = KMessageBox::questionYesNoCancel(this,
									     xi18nc("@info", "<p>\
This game contains level-specific sprite files.  If you edit the sprites using \
the sprite editor, you will only be editing the default sprites which are used for any \
levels that do not have specific sprite images.  For levels that do have \
specific sprites, changes made in the sprite editor will have no effect.\
</p>\
<p>\
To remove the level-specific sprite files, select <interface>Remove</interface>.  All levels in \
the game will then use the same sprite images;  these can be edited using the sprite \
editor in the usual way.  No files will be removed from the saved copy of the \
game until it is saved.\
</p>\
<p>\
To retain the level-specific sprite files, select <interface>Keep</interface>.  Changes made \
using the sprite editor may not have any effect on played levels.\
</p>"),
									     i18n("Multiple Sprites"),
									     KStandardGuiItem::remove(),
									     KGuiItem(i18n("Keep"), KStandardGuiItem::yes().icon()));
		if (q==KMessageBox::Cancel) return;
		if (q==KMessageBox::Yes) game->getSprites()->removeMultiLevels();
		editWarned = true;			// not again for this game
        }

	if (edit==NULL)
	{
		edit = new GameEditor(this);
		connect(edit,SIGNAL(editModified()),
			this,SLOT(updateEditModified()));
		connect(edit,SIGNAL(editMapsChange()),
			this,SLOT(updateEditLevels()));
	}

	passToEditor();
	edit->show();
}


void MainWindow::closeEvent(QCloseEvent *e)
{
	qDebug();

	if (edit!=NULL) edit->close();
	saveOptions();
	KMainWindow::closeEvent(e);
}


void MainWindow::fetchFromEditor()
{
	if (edit==NULL) return;
	qDebug() << "mod=" << edit->isModified();

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
	qDebug();

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

	qDebug() << "name='" << d.name() << "' path='" << d.path() << "'";

	QDir dir(d.path());
	if (dir.exists())
	{
		if (KMessageBox::warningContinueCancel(
			    this,i18n("<qt>Overwrite existing episode <b>%1</b>?", d.name()),
			    QString(),
			    KStandardGuiItem::overwrite())!=KMessageBox::Continue) return;
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
	qDebug();

	SelectGameDialog d(i18n("Remove Game"), this, true);
	if (!d.exec()) return;

	const Episode *e = d.selectedItem();
	if (e->isGlobal()) return;			// shouldn't happen

	const QString name = e->getName();
	qDebug() << "selected = '" << e->getName() << "'";

	if (KMessageBox::warningContinueCancel(
		    this,i18n("<qt>Are you sure you want to permanently delete the episode <b>%2</b>,"
                              "<br>located at '%1'?",
                              Episode::savePath(name),
                              name),
		    QString(),KStandardGuiItem::del())!=KMessageBox::Continue) return;

	qDebug() << "e=" << e << "current=" << currentepisode;

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
	qDebug();
        ImportWizard wiz(i18n("Import Assistant"),this);
        if (wiz.exec())
	{
		QString newEpisodeName = wiz.newEpisodeToLoad();
		if (newEpisodeName.isEmpty()) return;
		if (!queryClose()) return;
		loadGame(newEpisodeName);
	}
}


void MainWindow::slotExport()
{
	qDebug();
	KMessageBox::sorry(this,i18n("This option is not implemented yet.\nSee the TODO file for more information."));
}


void MainWindow::slotPrint()
{
	qDebug();
	KMessageBox::sorry(this,i18n("This option is not implemented yet.\nSee the TODO file for more information."));
}


void MainWindow::updateEditLevels()
{
	qDebug();

	fetchFromEditor();				// need to update 'lastlevel'
	updatePlayState(false,false);
}


void MainWindow::slotSelectCheats()
{
	qDebug() << "current" << cheats_used;

	CheatDialog d(i18n("Select Cheats"), this);
	d.setCheats(cheats_used);
	if (d.exec())
	{
		cheats_used = d.getCheats();
		qDebug() << "new" << cheats_used;
	        game->setCheats(cheats_used);
        }
}
