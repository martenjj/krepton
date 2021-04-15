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
#include <qstandardpaths.h>

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
#ifdef SND_PHONON
	mMediaObject->stop();
#endif
}


void Sound::playSound(Sound::Type s)
{
	if (!mEnabled) return;				// sounds not enabled
	if (mSoundDir.isEmpty()) return;		// no media to play

	qDebug() << "type" << s;

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
case Sound::Start:		name = "start";		break;
default:						return;
		}

		// First try the named sound file from the scheme
		QString fname = mSoundDir+'/'+name+SOUND_FILE_EXT;
		if (!QFile::exists(fname))
		{
			// Fallback from the 'start' sound to 'transport'
			if (s==Sound::Start) name = "transport";
			fname = mSoundDir+'/'+name+SOUND_FILE_EXT;
		}

		// Fallback from the current scheme to the default
		if (!QFile::exists(fname) && !mFallbackSoundDir.isEmpty())
		{
			if (s==Sound::Start) name = "start";
			fname = mFallbackSoundDir+'/'+name+SOUND_FILE_EXT;

			if (!QFile::exists(fname))
			{
				// Fallback from the 'start' sound to 'transport'
				if (s==Sound::Start) name = "transport";
				fname = mFallbackSoundDir+'/'+name+SOUND_FILE_EXT;
			}
		}

		// Have now tried all possibilities
		if (!QFile::exists(fname))
		{
			qWarning() << "Sound file does not exist" << fname;
			return;
		}

		QFileInfo fi(fname);
		if (fi.size()==0)			// check file is not empty
		{
#ifdef SND_PHONON
			// Create a dummy (empty) media object,
			// to simplify the caching and repeat logic
			src = new Phonon::MediaSource();
			qDebug() << "created empty media object for" << fname;
			mSourceMap[s] = src;
#else
			return;				// ignore for external player
#endif
		}

#ifdef SND_PHONON
		src = new Phonon::MediaSource(fname);
		qDebug() << "created media object for" << fname;
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


bool Sound::setSchemeName(const QString &name)
{
	mSoundDir = QString::null;			// not located yet

	mSoundScheme = name;
	if (mSoundScheme.isEmpty()) mSoundScheme = SOUND_DEFAULT_SCHEME;
	qDebug() << "set to" << mSoundScheme;

 	QString resfile = mSoundScheme;
	QString resource = QStandardPaths::locate(QStandardPaths::AppDataLocation, "sounds/"+resfile, QStandardPaths::LocateDirectory);
	if (resource.isEmpty())
	{
		qDebug() << "Cannot find sound resource for" << resfile << "- check installation!";
		return (false);
	}

// 	resource += mSoundScheme+"/";
// 	QDir d(resource);
// 	if (!d.exists())
// 	{
// 		qDebug() << "Sound scheme directory" << resource << "not found - check installation!";
// 		return (false);
// 	}

	mSoundDir = resource;				// sound directory in use

	if (name.isEmpty())				// setting the default...
	{
		mDefaultSoundDir = mSoundDir;		// record for use as fallback
		mFallbackSoundDir = QString::null;	// but no fallback for this
		qDebug() << "default sounds at" << mDefaultSoundDir;
	}
	else						// setting another...
	{
		mFallbackSoundDir = mDefaultSoundDir;	// fallback to the default
	}

	qDebug() << "sounds at" << mSoundDir;
	qDebug() << "fallback at" << mFallbackSoundDir;

#ifdef SND_PHONON
	qDeleteAll(mSourceMap);				// throw away media objects
	mSourceMap.clear();				// clear media object cache
	mLastPlayed = Sound::None;			// reset to start again
#endif
	return (true);
}


// TODO: lots in common with EpisodeList::EpisodeList()
QMap<QString,QString> Sound::allSchemesList()
{
	QMap<QString,QString> res;

	QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation,
	                                                   "sounds", QStandardPaths::LocateDirectory);
	for (const QString &d : qAsConst(dirs))
	{
		qDebug() << "sound dir" << d;

		QDir qd(d);
		if (!qd.exists())
		{
			reportError(ki18n("Sound directory '%1' not found"), d);
			continue;
		}
 
		qd.setFilter(QDir::Dirs);
		qd.setSorting(QDir::Name);
		const QFileInfoList list = qd.entryInfoList();
		for (QFileInfoList::const_iterator it = list.constBegin();
                     it!=list.constEnd(); ++it)
		{
                        QFileInfo fi = (*it);
			if (fi.fileName().startsWith(".")) continue;

			if (!qd.cd(fi.fileName()))
			{
				qWarning() << "cannot access directory:" << qd.absoluteFilePath(fi.fileName());
				continue;
			}

			QString dirname = qd.absolutePath();
			QString filename = dirname+"/info";
			QString schemename = qd.dirName();
			QFile f(filename);
			qd.cdUp();

			if (!f.open(QIODevice::ReadOnly))
			{
				qWarning() << "cannot read info file:" << filename;
				continue;
			}

			QTextStream t(&f);
			QString name;
			name = t.readLine().trimmed();
			f.close();

			qDebug() << "sound scheme at" << dirname << "-" << schemename << "=" << name;
			res[schemename] = name;
		}
	}

	return (res);
}


QString Sound::schemeConfigName() const
{
	return (mSoundScheme==SOUND_DEFAULT_SCHEME ? QString::null : mSoundScheme);
}
