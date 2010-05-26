////////////////////////////////////////////////////////////////////////////
//  
//  KRepton - the classic Repton game for KDE
//  
//  Copyright (c) 2008 Jonathan Marten <jjm@keelhaul.demon.co.uk>
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

#include <kdebug.h>
#include <klocale.h>

#include <qfile.h>
#include <qfileinfo.h>

#include "episodes.h"
#include "sprites.h"
#include "map.h"
#include "checkmap.h"

#include "importerbase.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor						//
//									//
//////////////////////////////////////////////////////////////////////////

ImporterBase::ImporterBase()
{
}

ImporterBase::~ImporterBase()
{
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Perform the import							//
//									//
//////////////////////////////////////////////////////////////////////////

bool ImporterBase::import(const QString &source,const QString &newName,QString *results)
{
    kdDebug(0) << k_funcinfo << "src=" << source << " name=" << newName << endl;

    QFile f(source);
    QFileInfo fi(f);

    if (!fi.exists())
    {
        *results = i18n("Source file <b>%1</b> does not exist").arg(fi.absFilePath());
        return (false);
    }
    if (!fi.isReadable())
    {
        *results = i18n("Source file <b>%1</b> is not readable").arg(fi.absFilePath());
        return (false);
    }

    unsigned int expectSize = formatInfo()->filesize;
    if (expectSize>0 && fi.size()!=expectSize)
    {
        *results = i18n("Source file <b>%3</b> is not the expected size.<br>File size is %1 bytes, expected %2 bytes.")
            .arg(fi.size()).arg(expectSize).arg(fi.absFilePath());
        return (false);
    }

    if (!f.open(QIODevice::ReadOnly))
    {
        *results = i18n("Cannot open file <b>%1</b>").arg(source);
        return (false);
    }

    Episode *episode = new Episode(newName,false,Episode::savePath(newName));
    Sprites *sprites = new Sprites;
    MapList maplist;
    maplist.setAutoDelete(true);

    bool status = doImport(f,episode,sprites,&maplist,results);

    f.close();
    if (!status)
    {
        if (results->isEmpty()) *results = i18n("Import failed");
        return (false);
    }

    CheckMap cm(maplist);				// do the consistency check
    switch (cm.status())
    {
case CheckMap::Ok:
    break;

case CheckMap::Warning:
    *results += i18n("Import completed, but with consistency check warnings:<p><ul>%1</ul>").arg(cm.detail());
    break;

case CheckMap::Fatal:
    *results += i18n("Import completed, but with consistency check errors:<p><ul>%1</ul>").arg(cm.detail());
    break;
    }

    if (!episode->saveInfoAndMaps(&maplist) || !sprites->save(episode))
    {							// save the new episode
        *results = i18n("Saving new episode <b>%1</b> failed").arg(newName);
        return (false);
    }

    EpisodeList::list()->add(episode);			// add to episode list
    return (true);					// import done
}
