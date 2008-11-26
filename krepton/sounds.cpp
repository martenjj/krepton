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
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kaudioplayer.h>

#include "krepton.h"

#include "sounds.h"


static QString sounddir = QString::null;		// directory for sound resources

bool Sound::enabled = false;				// global sound enable flag


void Sound::playSound(Sound::Type s)
{
	if (!enabled) return;
	kdDebug(0) << k_funcinfo << "type=" << s << endl;

	if (sounddir.isNull()) sounddir = KGlobal::dirs()->findResourceDir("sound","die.wav");

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
	KAudioPlayer::play(fname);
}
