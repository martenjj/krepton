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

#include "gameplayer.h"

#include <kmessagebox.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <qbitmap.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcursor.h>

#include "krepton.h"
#include "episodes.h"
#include "pixmaps.h"
#include "sprites.h"
#include "mapplay.h"
#include "checkmap.h"
#include "settings.h"


GamePlayer::GamePlayer(QWidget *parent)
        : QWidget(parent)
{
        setObjectName("GamePlayer");

	in_game = false;
	in_pause = false;
	in_timelimit = false;
	currentmap = NULL;
	currentlevel = -1;
	sprites = NULL;

	cheats_used = Cheat::NoCheats;
	cheats_ever_used = false;

	setFocusPolicy(Qt::StrongFocus);
	setFocus();

	timerObjects = startTimer(200);			// Update falling rocks or eggs
	timerMonsters = startTimer(300);		// Update monsters and spirits
	timerEggs = startTimer(500);			// Update a breaking egg
	timerSecs = startTimer(1000);			// Update elapsed time and idle Repton
	timerPlants = startTimer(3000);			// Update replicating plants
}

GamePlayer::~GamePlayer()
{
	killTimer(timerObjects);
	killTimer(timerMonsters);
	killTimer(timerEggs);
	killTimer(timerSecs);
	killTimer(timerPlants);

	if (currentmap!=NULL) delete currentmap;
	if (sprites!=NULL) delete sprites;

        qDebug() << "done";
}

const QString GamePlayer::loadEpisode(const Episode *e)
{
	qDebug() << "name" << e->getName();

	QString status = loadSprites(e);
	if (!status.isEmpty()) return (status);

	status = loadMaps(e);
	if (!status.isEmpty()) return (status);

	cheats_used = Cheat::NoCheats;			// reset for new episode
	cheats_ever_used = false;
	return ("");
}

const QString GamePlayer::loadSprites(const Episode *e)
{
	qDebug() << "name" << e->getName();

	setCursor(QCursor(Qt::WaitCursor));
	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(e);
	unsetCursor();

	if (in_game) sprites->prepare(currentlevel+1);
	return (sprites->loadStatus());
}

const QString GamePlayer::loadMaps(const Episode *e)
{
	qDebug() << "name" << e->getName();

	const QString status = e->loadMaps(&maps);
	if (!status.isEmpty()) return (status);

	currentlevel = -1;				// not valid any longer
	if (currentmap!=NULL) delete currentmap;
	currentmap = NULL;
	return ("");
}

const Episode *GamePlayer::saveEpisode(const QString name, const QString path)
{
	qDebug() << "name" << name << "path" << path;

	Episode *e = new Episode(name,false,path);	// construct new episode

	if (!(e->saveInfoAndMaps(&maps) && sprites->save(e)))
	{
		delete e;
		return (NULL);
	}

	EpisodeList::list()->add(e);			// add to episode list
	return (e);
}

const Episode *GamePlayer::saveEpisode(const Episode *e)
{
	qDebug() << "name" << e->getName();
	if (!(e->saveInfoAndMaps(&maps) && sprites->save(e))) return (NULL);

	EpisodeList::list()->add(e);			// ensure still in list
	return (e);
}

void GamePlayer::setSprites(const Sprites *ss)
{
	qDebug() << "sprites=" << sprites << " ss=" << ss;

	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(ss);
}

void GamePlayer::setMaps(const MapList ml)
{
	qDebug() << "count=" << ml.count();

        qDeleteAll(maps);
	maps.clear();

        for (MapList::const_iterator it = ml.constBegin();
             it!=ml.constEnd(); ++it)
        {
            const Map *mm = (*it);
            maps.append(new Map(*mm));
	}

// check how many levels now, may need to trim 'lastlevel'
	qDebug() << "count=" << maps.count() <<
		" last=" << currentlevel;
	if (currentlevel>static_cast<int>((maps.count()-1))) currentlevel = -1;
	if (currentmap!=NULL) delete currentmap;
	currentmap = NULL;
}


void GamePlayer::newGame()
{
	lives = 3;
	points = 0;
}


void GamePlayer::startGame(const Episode *e,int level)
{
	if (e!=NULL)					// starting a new game
	{
		CheckMap cm(maps);
		if (cm.status()==CheckMap::Fatal)
		{
			KMessageBox::detailedSorry(this,"There are problems with the episode (use the editor consistency check for more information). They must be fixed before you can play the game.",
						   cm.detail("","\n"));
			return;
		}

		episodeName = e->getName();
		newGame();
		emit changedGameState(true);
	}

	qDebug() << "name='" << episodeName << "' level=" << level;
	if (level>=int(maps.count())) return;

	if (currentmap!=NULL) delete currentmap;
	currentmap = new MapPlay(*maps.at(level));
	sprites->prepare(level+1);

	if (cheats_used!=Cheat::NoCheats) currentmap->setCheats(cheats_used);

	currentlevel = level;
	seconds = currentmap->getSeconds();
	idle = 0;
	diamonds = 0;
	havekey = false;
	havecrown = false;
	pendingShuffle = 0;

	QString msg = i18n("<qt>Starting level %1 of episode <b>%2</b>", level+1, episodeName);
	if (level>0) msg += i18n("<br>The password is \"<b>%1</b>\"", currentmap->getPassword());
	KMessageBox::information(this,msg);

        recordLevel(GamePlayer::Started);		// record level as started

	in_game = true;
	in_pause = false;
	in_timelimit = false;

	emit changedStats(diamonds,seconds,points);
	emit changedPlayState(in_game,in_pause);
	emit changedLives(lives);
	emit changedFlags(havekey,havecrown);
	currentmap->startGame();
}


void GamePlayer::recordLevel(GamePlayer::State state)
{
        QString cg = "Episode "+Episode::sanitisedName(episodeName);
	KConfigGroup grp = KSharedConfig::openConfig()->group(cg);

	QString levelKey = QString::number(currentlevel);
	int oldstate = grp.readEntry(levelKey, -1);
	int newstate = static_cast<int>(state);

        // Only increase (improve) the level play/finish state, never lower it.
        if (newstate>oldstate) grp.writeEntry(levelKey, newstate);

	// But record the currently playing level anyway.
        if (state==GamePlayer::Started) grp.writeEntry("Playing", currentlevel);
}


// each is string of form "status level password"
QStringList GamePlayer::listLevels(const Episode *e)
{
	qDebug();

        QString cg = "Episode "+Episode::sanitisedName(e->getName());
	const KConfigGroup grp = KSharedConfig::openConfig()->group(cg);
	int playing = grp.readEntry("Playing", -1);

	QStringList result;
	for (int l = 0; l<static_cast<int>(maps.count()); ++l)
	{
		GamePlayer::State state = static_cast<GamePlayer::State>(grp.readEntry(QString::number(l), static_cast<int>(GamePlayer::Unplayed)));
		if (l==playing && state!=GamePlayer::Finished) state = GamePlayer::Playing;
		result << QString("%1 %2 %3").arg(state).arg(l).arg(maps.at(l)->getPassword());
        }

	return (result);
}


void GamePlayer::startGamePassword(const Episode *e,const QString& password)
{
	for (int i = 0; i<maps.count(); ++i)
        {
		const Map *mm = maps[i];
		if (QString::compare(mm->getPassword(), password, Qt::CaseInsensitive)==0)
		{
			startGame(e,i);
			return;
		}
	}

	KMessageBox::sorry(this,"Unknown password");
}


void GamePlayer::timerEvent(QTimerEvent *e)
{
	if (!in_game || in_pause) return;

	if (currentmap->hasDied() || currentmap->hasEndedLevel())
	{
		in_game = false;
		emit changedPlayState(in_game,in_pause);

		if (currentmap->hasDied()) endedGame(currentmap->howDied());
		else
		{
                        recordLevel(GamePlayer::Finished);
							// record level as completed
			if (currentlevel<static_cast<int>(maps.count()-1))
			{
				KMessageBox::information(this, i18n("Level %1 of episode <b>%2</b> completed!", currentlevel+1, episodeName));
				startGame(NULL, currentlevel+1);
			}
			else
			{
				KMessageBox::information(this, i18n("Episode <b>%1</b> finished!", episodeName));
				emit changedGameState(false);
				emit gameOver();
			}
		}

		repaint();
		return;
	}

	const int oldsecs = seconds;
	const int oldpoints = points;
	seconds += currentmap->gameSeconds();		// anything gained this tick
	points += currentmap->gamePoints();

	bool needsRepaint = false;
	const int id = e->timerId();
	if (id==timerObjects)
	{
		needsRepaint = currentmap->updateObjects();

		// The previous updateObjects() will have allowed moveable
		// objects (rocks and eggs) to fall by one square.  Now,
		// if doing an automatic Repton shuffle, move Repton back
		// and take any appropriate action before they continue
		// to fall on the next timer tick.
		if (pendingShuffle!=0)
		{
			if (pendingShuffle==Qt::Key_Right) needsRepaint |= goRight();
			else if (pendingShuffle==Qt::Key_Left) needsRepaint |= goLeft();
			pendingShuffle = 0;
		}
	}
	else if (id==timerMonsters)
	{
		needsRepaint = currentmap->updateMonsters();
	}
	else if (id==timerEggs)
	{
		needsRepaint = (currentmap->updateEggs() && currentmap->updateObjects());
	}
	else if (id==timerSecs)
	{
		// I'm not sure how much time limit warning was given by the
		// original BBC version, but here the screen starts to flash
		// when there are 20 seconds of the time limit remaining and the
		// flash to white is shown on even seconds (implemented by the
		// "!(seconds & 1)" in paintEvent() below).  There will therefore
		// be ten flashes before the time finally runs out.

		--seconds;				// count down level time limit
		if (seconds<=0)
		{
			if (!(cheats_used & Cheat::NoTimeLimit))
			{
				endedGame("Your time is up!");
				return;
			}

			seconds = 0;
			in_timelimit = false;
		}
		else in_timelimit = (seconds<=20);	// warn when 20 seconds left,
							// which will give 10 flashes
		if (!Settings::flashForTimeLimit()) in_timelimit = false;
		if (in_timelimit) needsRepaint = true;

		++idle;					// count up standing still time
		if (idle>10) needsRepaint |= currentmap->updateIdle();
	}
	else if (id==timerPlants)
	{
		needsRepaint = currentmap->updatePlants();
	}

	if (needsRepaint) repaint();

	const int gotdiamonds = currentmap->getDiamonds();
	if (gotdiamonds!=diamonds || seconds!=oldsecs || points!=oldpoints)
	{
		diamonds = gotdiamonds;
		emit changedStats(diamonds,seconds,points);
	}

	const bool gotkey = currentmap->haveKey();
	const bool gotcrown = currentmap->haveCrown();
	if (gotkey!=havekey || gotcrown!=havecrown)
	{
		havekey = gotkey;
		havecrown = gotcrown;
		emit changedFlags(havekey,havecrown);
	}
}

void GamePlayer::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	if (!in_game)
	{
		const QPixmap bgpix = Pixmaps::find(Pixmaps::Back);
		if (!bgpix.isNull())
                {
                        for (int y = 0; y<=(height()/bgpix.height()); ++y)
                        {
                                for (int x = 0; x<=(width()/bgpix.width()); ++x)
                                {
                                        p.drawPixmap(x*(bgpix.width()),
                                                     y*(bgpix.height()), bgpix);

                                }
                        }
                }
                return;
        }

	Sprites::GetFlag spriteFlag = Sprites::GetNormal;
	if (in_pause) spriteFlag = Sprites::GetGrey;
	else if (in_timelimit & !(seconds & 1)) spriteFlag = Sprites::GetBright;

	currentmap->paintMap(&p, width(), height(), sprites, spriteFlag);
}


bool GamePlayer::goUp()
{
	if (!in_game || in_pause) return (false);
	idle = 0;
	return (currentmap->goUp());
}

bool GamePlayer::goDown()
{
	if (!in_game || in_pause) return (false);
	idle = 0;
	return (currentmap->goDown());
}

bool GamePlayer::goLeft()
{
	if (!in_game || in_pause) return (false);
	idle = 0;
	return (currentmap->goLeft());
}

bool GamePlayer::goRight()
{
	if (!in_game || in_pause) return (false);
	idle = 0;
	return (currentmap->goRight());
}


void GamePlayer::keyPressEvent(QKeyEvent *e)
{
	bool needsRepaint = false;
	switch (e->key())
	{
case Qt::Key_Up:
		needsRepaint = goUp();
		break;

case Qt::Key_Down:
		needsRepaint = goDown();
		break;

case Qt::Key_Left:
		if (e->modifiers() & Qt::ShiftModifier) pendingShuffle = Qt::Key_Right;
		needsRepaint = goLeft();
		break;

case Qt::Key_Right:
		if (e->modifiers() & Qt::ShiftModifier) pendingShuffle = Qt::Key_Left;
		needsRepaint = goRight();
		break;

default:	e->ignore();
		return;
	}

	if (needsRepaint) repaint();
}


void GamePlayer::endedGame(const QString &how,bool suicide)
{
	qDebug() << "level=" << currentlevel;

	in_game = false;
	in_pause = false;
	in_timelimit = false;
	emit changedPlayState(in_game,in_pause);

	--lives;					// count down this life

	QString remain;
	if (lives==0) remain = i18n("You have no lives remaining.");
	else remain = i18np("You have 1 life remaining.",
			    "You have %1 lives remaining.", lives);
	QString msg = i18n("<qt>You died...<br><b>%1</b><p>%2", how, remain);

	bool restartLevel = false;

	if (suicide && lives>0)				// option to continue
	{
		// Player killed themselves with more lives remaining.
		// Options are Continue, Restart, Finish.
		msg += i18n("<p>Do you want to continue the game?");
		KMessageBox::ButtonCode q = KMessageBox::questionYesNoCancel(this, msg, QString(),
									     KStandardGuiItem::cont(),
									     KGuiItem(i18n("Restart"), KStandardGuiItem::reset().icon()),
									     KGuiItem(i18n("Finish"), KStandardGuiItem::stop().icon()));
		if (q==KMessageBox::Cancel) lives = 0;
		else if (q==KMessageBox::No) restartLevel = true;
	}
	else
	{
		if (lives==0)
		{
			// Player died or killed themselves with no more
			// lives remaining.  Options are Restart and Finish.
			KMessageBox::ButtonCode q = KMessageBox::questionYesNo(this, msg, QString(),
									       KGuiItem(i18n("Restart"), KStandardGuiItem::reset().icon()),
									       KGuiItem(i18n("Finish"), KStandardGuiItem::stop().icon()));
			if (q==KMessageBox::Yes) restartLevel = true;
		}
		else
		{
			// Player died with more lives remaining.
			// Options are Continue and Restart.
			KMessageBox::ButtonCode q = KMessageBox::questionYesNo(this, msg, QString(),
									     KStandardGuiItem::cont(),
									     KGuiItem(i18n("Restart"), KStandardGuiItem::reset().icon()));
			if (q==KMessageBox::No) restartLevel = true;
		}
	}

	if (restartLevel)				// restarting from the beginning
	{
		newGame();				// reset lives and points
		startGame(NULL, currentlevel);		// current episode and level
		return;
	}

	if (lives==0)					// not continuing the game
	{
		emit changedGameState(false);
		emit gameOver();
	}
	else
	{						// continuing the game
		if (seconds<=0) seconds = currentmap->getSeconds();
		currentmap->restartGame();

		in_game = true;
		emit changedLives(lives);
		emit changedPlayState(in_game,in_pause);
	}
}


void GamePlayer::pauseAction()
{
	if (!in_game) return;

	in_pause = !in_pause;
	emit changedPlayState(in_game,in_pause);
	repaint();
}


void GamePlayer::suicideAction()
{
	if (!in_game) return;
	endedGame("You killed yourself!",true);
}


void GamePlayer::setCheats(Cheat::Options cheats)
{
	qDebug() << "cheats" << cheats;
	cheats_used = cheats;
	if (cheats!=Cheat::NoCheats) cheats_ever_used = true;

        if (currentmap!=NULL) currentmap->setCheats(cheats_used);
}
