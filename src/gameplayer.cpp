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

#include <kapp.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstandarddirs.h>

#include <qbitmap.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcursor.h>

#include "krepton.h"
#include "episodes.h"
#include "pixmaps.h"
#include "sprites.h"
#include "map1.h"
#include "checkmap.h"

#include "gameplayer.h"
#include "gameplayer.moc"


GamePlayer::GamePlayer(QWidget *parent,const char *name)
        : QWidget(parent,name,Qt::WRepaintNoErase)
{
	in_game = false;
	in_pause = false;
	currentmap = NULL;
	currentlevel = -1;
	sprites = NULL;

	setFocusPolicy(QWidget::StrongFocus);
	setFocus();

	timerObjects = startTimer(200);
	timerMonsters = startTimer(300);
	timerEggs = startTimer(500);
	timerSecs = startTimer(1000);
	timerPlants = startTimer(3000);
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
}

const QString GamePlayer::loadEpisode(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

	const QString status = loadSprites(e);
	if (!status.isNull()) return (status);
	return (loadMaps(e));
}

const QString GamePlayer::loadSprites(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

	setCursor(QCursor(Qt::WaitCursor));
	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(e);
	unsetCursor();

	return (sprites->loadStatus());
}

const QString GamePlayer::loadMaps(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

	const QString status = e->loadMaps(&maps);
	if (!status.isNull()) return (status);

	currentlevel = -1;				// not valid any longer
	if (currentmap!=NULL) delete currentmap;
	currentmap = NULL;
	return (QString::null);
}

const Episode *GamePlayer::saveEpisode(const QString name,const QString path)
{
	kdDebug(0) << k_funcinfo << "AS name='" << name << "' path='" << path << "'" << endl;

	Episode *e = new Episode(name,false,path);	// construct new episode

	if (!(e->saveInfoAndMaps(&maps) && sprites->save(e)))
	{
		delete e;
		return (false);
	}

	EpisodeList::list()->add(e);			// add to episode list
	return (e);
}

const Episode *GamePlayer::saveEpisode(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;
	if (!(e->saveInfoAndMaps(&maps) && sprites->save(e))) return (NULL);

	EpisodeList::list()->add(e);			// ensure still in list
	return (e);
}

void GamePlayer::setSprites(const Sprites *ss)
{
	kdDebug(0) << k_funcinfo << "sprites=" << ((void*)sprites) << " ss=" << ((void*)ss) << endl;

	if (sprites!=NULL) delete sprites;
	sprites = new Sprites(*ss);
}

void GamePlayer::setMaps(const MapList ml)
{
	kdDebug(0) << k_funcinfo << "count=" << ml.count() << endl;

	maps.setAutoDelete(true);
	maps.clear();
	maps.setAutoDelete(false);

	MapListIterator mi(ml);
	for (const Map *mm; (mm = mi.current())!=NULL; ++mi)
	{
		maps.append(new Map(*mm));
	}

// check how many levels now, may need to trim 'lastlevel'
	kdDebug(0) << k_funcinfo << "count=" << maps.count() <<
		" last=" << currentlevel << endl;
	if (currentlevel>((int) (maps.count()-1))) currentlevel = -1;
	if (currentmap!=NULL) delete currentmap;
	currentmap = NULL;
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
		lives = 3;
		points = 0;
		emit changedGameState(true);
	}

	kdDebug(0) << k_funcinfo << "name='" << episodeName << "' level=" << level << endl;
	if (level>=int(maps.count())) return;

	if (currentmap!=NULL) delete currentmap;
	currentmap = new MapPlay(*maps.at(level));
        sprites->prepare(level+1);

	currentlevel = level;
	seconds = currentmap->getSeconds();
	idle = 0;
	diamonds = 0;
	havekey = false;
	havecrown = false;

	QString msg = QString("<qt>Starting level %1 of episode <b>%2</b>").arg(level+1).arg(episodeName);
	if (level>0) msg += QString("<br>The password is \"<b>%1</b>\"").arg(currentmap->getPassword());
	KMessageBox::information(this,msg);

        recordLevel(GamePlayer::Started);		// record level as started

	in_game = true;
	in_pause = false;
	emit changedPlayState(in_game,in_pause);
	emit changedLives(lives);
	emit changedFlags(havekey,havecrown);
	currentmap->startGame();
}


void GamePlayer::recordLevel(GamePlayer::State state)
{
	KConfig *conf = KGlobal::config();
        QString cg = "Episode "+Episode::sanitisedName(episodeName);
        conf->setGroup(cg);
        conf->writeEntry(QString::number(currentlevel),state);
        if (state==GamePlayer::Started) conf->writeEntry("Playing",currentlevel);
}


// each is string of form "status level password"
QStringList GamePlayer::listLevels(const Episode *e)
{
	kdDebug() << k_funcinfo << endl;

	KConfig *conf = KGlobal::config();
        conf->setGroup("Episode "+Episode::sanitisedName(e->getName()));
	int playing = conf->readNumEntry("Playing",-1);

	QStringList result;
	for (int l = 0; l<((int) maps.count()); ++l)
	{
		GamePlayer::State state = static_cast<GamePlayer::State>(conf->readNumEntry(QString::number(l),GamePlayer::Unplayed));
		if (l==playing && state!=GamePlayer::Finished) state = GamePlayer::Playing;
		result << QString("%1 %2 %3").arg(state).arg(l).arg(maps.at(l)->getPassword());
        }

	return (result);
}


void GamePlayer::startGamePassword(const Episode *e,const QString& password)
{
	for (const Map *mm = maps.first(); mm!=NULL; mm = maps.next())
	{
		if (mm->getPassword().lower()==password.lower())
		{
			startGame(e,maps.at());
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
			if (currentlevel<(maps.count()-1))
			{
				KMessageBox::information(this,"Level finished!");
				startGame(NULL,currentlevel+1);
			}
			else
			{
				KMessageBox::information(this,"Episode finished!");
				emit changedGameState(false);
				emit gameOver();
			}
		}

		repaint(false);
		return;
	}

	const int oldsecs = seconds;
	const int oldpoints = points;
	seconds += currentmap->readSeconds();		// anything gained this tick
	points += currentmap->readPoints();

	int id = e->timerId();
	if (id==timerObjects)
	{
		if (currentmap->updateObjects()) repaint(false);
	}
	else if (id==timerMonsters)
	{
		if (currentmap->updateMonsters()) repaint(false);
	}
	else if (id==timerEggs)
	{
		if (currentmap->updateEggs() && currentmap->updateObjects()) repaint(false);
	}
	else if (id==timerSecs)
	{
		if (--seconds<=0) endedGame("Your time is up!");

		++idle;
		if (idle>10 && currentmap->updateIdle()) repaint(false);
	}
	else if (id==timerPlants)
	{
		if (currentmap->updatePlants()) repaint(false);
	}

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
	if (!in_game)
	{
		const QPixmap bgpix = Pixmaps::find(Pixmaps::Back);
		if (bgpix.isNull()) return;

		for (int y = 0; y<=(height()/bgpix.height()); ++y)
		{
			for (int x = 0; x<=(width()/bgpix.width()); ++x)
			{
				bitBlt(this,x*(bgpix.width()),y*(bgpix.height()),&bgpix);
			}
		}
		return;
	}

	QPixmap pm(size());
	QPainter p(&pm);
	currentmap->paintMap(&p,width(),height(),sprites);

	if (in_pause)
	{
		const QPixmap pausepix = Pixmaps::find(Pixmaps::Pause);
		p.drawPixmap((width()-pausepix.width())/2, 
			     (height()-pausepix.height())/2,pausepix);
	}

	p.end();
	bitBlt(this,0,0,&pm);
}

void GamePlayer::goUp()
{
	if (!in_game || in_pause) return;
	if (currentmap->goUp()) repaint(false);
	idle = 0;
}

void GamePlayer::goDown()
{
	if (!in_game || in_pause) return;
	if (currentmap->goDown()) repaint(false);
	idle = 0;
}

void GamePlayer::goLeft()
{
	if (!in_game || in_pause) return;
	if (currentmap->goLeft()) repaint(false);
	idle = 0;
}

void GamePlayer::goRight()
{
	if (!in_game || in_pause) return;
	if (currentmap->goRight()) repaint(false);
	idle = 0;
}

void GamePlayer::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
case Key_Up:	goUp();
		break;

case Key_Down:	goDown();
		break;

case Key_Left:	goLeft();
		break;

case Key_Right:	goRight();
		break;

default:	e->ignore();
		break;
	}
}


void GamePlayer::endedGame(const QString &how,bool suicide)
{
	kdDebug(0) << k_funcinfo << "level=" << currentlevel << endl;

	in_game = false;
	in_pause = false;
	emit changedPlayState(in_game,in_pause);

	--lives;					// count down this life

	QString msg = QString("<qt>You died...<br><b>%1</b>").arg(how);
	QString liv = "no";
	if (lives>0) liv.setNum(lives);
	msg += QString("<p>You have <b>%1</b> %2 remaining.")
		.arg(liv).arg(lives==1 ? "life" : "lives");

	if (suicide && lives>0)				// option to continue
	{
		msg += QString("<p>Do you want to continue the game?");
		if (KMessageBox::questionYesNo(this,msg,QString::null,
					       KGuiItem("&Continue"),
					       KGuiItem("&Finish"))!=KMessageBox::Yes)
		{
			lives = 0;
		}
	}
	else KMessageBox::sorry(this,msg);

	if (lives==0)					// not continuing the game
	{
		emit changedGameState(false);
		emit gameOver();
	}
	else
	{						// reset if timed out
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
	repaint(false);
}

void GamePlayer::suicideAction()
{
	if (!in_game) return;
	endedGame("You killed yourself!",true);
}
