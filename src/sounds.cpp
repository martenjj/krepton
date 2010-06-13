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

#include <kglobal.h>
#include <kstandarddirs.h>

#include "krepton.h"

#include "sounds.h"

#ifdef SND_PLAYOBJECT
#include <arts/kartsdispatcher.h>
#include <arts/kartsserver.h>
#include <arts/kplayobjectfactory.h>
#endif
#ifdef SND_AUDIOPLAYER
#include <kaudioplayer.h>
#endif
#ifdef SND_EXTERNAL
#include <kprocess.h>
#endif

static QString sounddir = QString::null;		// directory for sound resources
bool Sound::enabled = false;				// global sound enable flag

#ifdef SND_PLAYOBJECT
KDE::PlayObject *Sound::playObject = NULL;
int Sound::lastPlayed = -1;
#endif

void Sound::playSound(Sound::Type s)
{
	if (!enabled) return;
	kDebug() << "type=" << s << endl;

	if (sounddir.isNull())
	{
		sounddir = KGlobal::dirs()->findResourceDir("sound","die.wav");
		kDebug() << "sounds at " << sounddir << endl;
	}

	QString name;
	switch (s)
	{
case Sound::Die:		name = "die";		break;
case Sound::Got_Diamond:	name = "diamond";	break;
case Sound::Got_Crown:		name = "crown";		break;
case Sound::Cage_Blip:		name = "cage";		break;
case Sound::Got_Key:		name = "key";		break;
case Sound::Got_Time:		name = "time";		break;
case Sound::Transport:		name = "transport";	break;
case Sound::Kill_Monster:	name = "monster";	break;
case Sound::Broken_Egg:		name = "egg";		break;
	}

	QString fname = sounddir+name+".wav";

#ifdef SND_PLAYOBJECT
	if (playObject!=NULL)				// have existing player
	{
		if (lastPlayed!=s)			// but not for this sound
		{
			delete playObject;
		}
		else
		{
			if (playObject->state()!=Arts::posPlaying)
			{
				playObject->play();
				return;
			}
		}
	}

	KArtsDispatcher dispatcher;
	KArtsServer server;
	KDE::PlayObjectFactory factory(server.server());
	playObject = factory.createPlayObject(fname,"audio/x-wav",true);
	if (playObject!=NULL) playObject->play();
#endif

#ifdef SND_AUDIOPLAYER
	KAudioPlayer::play(fname);
#endif

#ifdef SND_EXTERNAL
	KProcess *proc = new KProcess(NULL);
#ifdef EXT_ARTSPLAY
	*proc << "artsplay" << fname;
#endif
#ifdef EXT_SOXPLAY
	*proc << "play" << fname;
#endif
#ifdef EXT_ALSAPLAY
	*proc << "aplay" << "-q" << fname;
#endif
	proc->start(KProcess::DontCare);
#endif

}
