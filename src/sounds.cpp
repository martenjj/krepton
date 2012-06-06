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
#include "sounds.h"

#include <qdir.h>
#include <qfile.h>

#include <kglobal.h>
#include <kstandarddirs.h>

#ifdef SND_PHONON
#include <Phonon/MediaSource>
#include <Phonon/AudioOutput>
#endif

#ifdef SND_EXTERNAL
#include <kprocess.h>
#endif

#ifndef SOUND_FILE_EXT
#define SOUND_FILE_EXT		".wav"
#endif
#define SOUND_DEFAULT_SCHEME	"default"


static Sound *sInstance = NULL;


Sound *Sound::self()
{
	if (sInstance==NULL) sInstance = new Sound();
	return (sInstance);
}


Sound::Sound()
{
	mEnabled = true;
	setSchemeName(QString::null);			// find sounds for default scheme

#ifdef SND_PHONON
	mLastPlayed = Sound::None;

        mMediaObject = new Phonon::MediaObject(NULL);
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::GameCategory, NULL);
	Phonon::createPath(mMediaObject, audioOutput);
#endif
}


Sound::~Sound()
{
}


void Sound::playSound(Sound::Type s)
{
	if (!mEnabled) return;				// sounds not enabled
	if (mSoundDir.isEmpty()) return;		// no media to play

	kDebug() << "type" << s;

#ifdef SND_PHONON
	if (s==mLastPlayed)				// same as last sound
	{
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
case Sound::Diamond:		name = "diamond";	break;
case Sound::Crown:		name = "crown";		break;
case Sound::Cage:		name = "cage";		break;
case Sound::Key:		name = "key";		break;
case Sound::Time:		name = "time";		break;
case Sound::Transport:		name = "transport";	break;
case Sound::Monster:		name = "monster";	break;
case Sound::Egg:		name = "egg";		break;
default:						return;
		}

		QString fname = mSoundDir+name+SOUND_FILE_EXT;
		if (!QFile::exists(fname))
		{
			kDebug() << "Sound file does not exist" << fname;
			return;
		}

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


void Sound::setSchemeName(const QString &name)
{
	mSoundDir = QString::null;			// not located yet

	mSoundScheme = name;
	if (mSoundScheme.isEmpty()) mSoundScheme = SOUND_DEFAULT_SCHEME;
	kDebug() << "set to" << mSoundScheme;

	QString resfile = (QString(SOUND_DEFAULT_SCHEME)+"/info");
	QString resource = KGlobal::dirs()->findResourceDir("sound", resfile);
	if (resource.isEmpty())
	{
		kDebug() << "Cannot find sound resource for" << resfile << "- check installation!";
		return;
	}

	resource += mSoundScheme+"/";
	QDir d(resource);
	if (!d.exists())
	{
		kDebug() << "Sound scheme directory" << resource << "not found - check installation!";
		return;
	}

	mSoundDir = resource;
	kDebug() << "sounds at" << mSoundDir;

#ifdef SND_PHONON
	qDeleteAll(mSourceMap);				// clear media object cache
#endif
}
