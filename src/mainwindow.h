///////////////// -*- mode:c++; indent-tabs-mode:t; c-basic-offset:8 -*- ///
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kxmlguiwindow.h>


class GamePlayer;
class GameEditor;
class Episode;

class QAction;
class KToggleAction;
class KSelectAction;

class QWidget;
class QLabel;


class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = NULL);
	virtual ~MainWindow() = default;

	void loadGame(const Episode *e);
	void loadGame(const QString name);

protected:
	void readOptions();
	void saveOptions();
	bool queryClose() override;
	void closeEvent(QCloseEvent *e) override;

protected slots:
	void slotSelectGame();
	void slotStartGame();
	void slotRestartGame();
	void slotContinueGame();
	void slotLoadSprites();
	void slotPauseGame();
	void slotSuicide();
	void slotSetMagnification(int idx);
	void slotSelectCheats();
	void slotEdit();
	void slotSave();
	void slotSaveAs();
	void slotImport();
	void slotExport();
	void slotPrint();
	void slotRemove();
#ifdef HAVE_LIBKDEGAMES
	void slotHighScores();
#endif
	void slotSoundsEnable();
	void slotSoundsScheme(QAction *act);

	void updateEditModified();
	void updateEditLevels();

	void updatePlayState(bool ingame,bool inpause);
	void updateGameState(bool playing);
	void updateStats(int diamonds,int secs = -1,int points = -1);
	void updateFlags(bool key,bool crown);
	void updateLives(int lives);
	void gameOver();

private:
	QAction *pauseAction;
	KToggleAction *soundsEnableAction;
	QAction *selectAction;
	QAction *startAction;
	QAction *restartAction;
	QAction *continueAction;
	QAction *suicideAction;
	QAction *loadspritesAction;
	KSelectAction *magnificationList;
	QAction *cheatsAction;
	QAction *editAction;
	QAction *saveAction;
	QAction *saveAsAction;
	QAction *removeAction;
	QAction *importAction;
	QAction *exportAction;
	QAction *printAction;
	KSelectAction *soundsSchemeList;

	KToggleAction *strictToggle;
	KToggleAction *flashToggle;

	GamePlayer *game;
	GameEditor *edit;
	const Episode *currentepisode;
	bool modified;
	bool editWarned;
	Cheat::Options cheats_used;

	QLabel *keyflag;				// toolbar icons
	QLabel *crownflag;
	QLabel *livesflag;
	QLabel *diamondsCount;
	QLabel *timeDisplay;
	QLabel *scoreDisplay;

	const QString prepareGame(const Episode *e,bool spritesonly = false);
	bool prepareSave();
	void updateCaption();
	void fetchFromEditor();
	void passToEditor();
	void updateSoundsMenu();
};

#endif							// !MAINWINDOW_H
