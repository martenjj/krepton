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

#ifndef SOUNDS_H
#define SOUNDS_H

#include "krepton.h"


class Sound
{

public:
	enum Type
	{
		Cage_Blip,
		Die,
		Broken_Egg,
		Got_Crown,
		Got_Diamond,
		Got_Key,
		Got_Time,
		Kill_Monster,
		Transport
	};

	static void setEnabled(bool e) { enabled = e; }
	static bool isEnabled() { return (enabled); }
	static void playSound(Sound::Type s);

private:
	static bool enabled;
};


#endif							// !SOUNDS_H
