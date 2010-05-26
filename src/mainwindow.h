////////////////////////////////////////////////////// -*- mode:c++; -*- ///
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

#include <kmainwindow.h>


class GamePlayer;
class GameEditor;
class Episode;

class KAction;
class KToggleAction;
class KSelectAction;

class QWidget;
class QLabel;


class MainWindow : public KMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = NULL,const char *name = NULL);
	~MainWindow();

	void loadGame(const Episode *e);
	void loadGame(const QString name);

protected:
	void readOptions();
	void saveOptions();
	bool queryClose();
	void closeEvent(QCloseEvent *e);

protected slots:
	void slotSelectGame();
	void slotStartGame();
	void slotRestartGame();
	void slotContinueGame();
	void slotLoadSprites();
	void slotPauseGame();
	void slotSuicide();
//	void slotFinish();
	void slotSetMagnification();
	void slotEdit();
	void slotSave();
	void slotSaveAs();
	void slotImport();
	void slotExport();
	void slotPrint();
	void slotRemove();
	void slotHighScores();
	void slotSoundsChanged();
	void slotAlignChanged();

	void slotStrictCheck();
	void slotSpriteEditor();
	void slotLevelEditor();
	void slotDataEditor();
	void slotRealignEditor();
	void updateWindowStates();
	void updateEditModified();
	void updateEditLevels();

	void updatePlayState(bool ingame,bool inpause);
	void updateGameState(bool playing);
	void updateStats(int diamonds,int secs = -1,int points = -1);
	void updateFlags(bool key,bool crown);
	void updateLives(int lives);
	void gameOver();

private:
	KToggleAction *pauseAction;
	KToggleAction *soundsAction;
	KAction *selectAction;
	KAction *startAction;
	KAction *restartAction;
	KAction *continueAction;
	KAction *suicideAction;
//	KAction *finishAction;
	KAction *loadspritesAction;
	KSelectAction *magnificationList;
	KAction *editAction;
	KAction *saveAction;
	KAction *saveAsAction;
	KAction *removeAction;
	KAction *importAction;
	KAction *exportAction;
	KAction *printAction;

	KToggleAction *strictToggle;
#ifdef EDITOR_3_WINDOWS
	KToggleAction *spriteToggle;
	KToggleAction *mapToggle;
	KToggleAction *dataToggle;
	KToggleAction *alignToggle;
	KAction *realignAction;
#else
	KAction *spriteAction;
	KAction *mapAction;
	KAction *dataAction;
#endif
	KAction *checkAction;

	GamePlayer *game;
	GameEditor *edit;
	const Episode *currentepisode;
	bool modified;
	bool editWarned;

	QLabel *keyflag;				// toolbar icons
	QLabel *crownflag;
	QLabel *livesflag;

	const QString prepareGame(const Episode *e,bool spritesonly = false);
	bool prepareSave();
	void updateCaption();
	void fetchFromEditor();
	void passToEditor();
};

#endif							// !MAINWINDOW_H
