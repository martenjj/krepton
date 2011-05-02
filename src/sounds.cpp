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

#ifdef SND_PHONON
#include <Phonon/MediaSource>
#include <Phonon/AudioOutput>
#endif

#include "krepton.h"

#include "sounds.h"

#ifdef SND_EXTERNAL
#include <kprocess.h>
#endif


static Sound *sInstance = NULL;


Sound *Sound::self()
{
	if (sInstance==NULL) sInstance = new Sound();
	return (sInstance);
}


Sound::Sound()
{
	mEnabled = true;

        mSoundDir = KGlobal::dirs()->findResourceDir("sound","die.wav");
	kDebug() << "sounds at " << mSoundDir;

	mLastPlayed = Sound::None;

#ifdef SND_PHONON
        mMediaObject = new Phonon::MediaObject(NULL);
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::GameCategory, NULL);
	Phonon::createPath(mMediaObject, audioOutput);
#endif
}


Sound::~Sound()
{
	kDebug();
}


void Sound::playSound(Sound::Type s)
{
	if (!mEnabled) return;
	kDebug() << "type=" << s << endl;

#ifdef SND_PHONON
	if (s==mLastPlayed)
	{
kDebug() << "last";
		mMediaObject->play();
		return;
	}

	Phonon::MediaSource *src = mSourceMap.value(s);
	if (src==NULL)
	{
#endif
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
default:						return;
		}

		QString fname = mSoundDir+name+".wav";
#ifdef SND_PHONON
		src = new Phonon::MediaSource(fname);
		kDebug() << "created media object for" << fname;
		mSourceMap[s] = src;
	}

	if (src!=NULL)
	{
		mMediaObject->setCurrentSource(*src);
		mMediaObject->play();
		mLastPlayed = s;
		return;
	}
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
