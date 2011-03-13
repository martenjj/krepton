/////////////////////////////////////////////////// -*- mode:c++; -*- ////
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

#ifndef IMPORTERBASE_H
#define IMPORTERBASE_H

#include <qstring.h>

#include "importmanager.h"


class QFile;
class Episode;
class Sprites;


class ImporterBase					// abstract
{
public:
    ImporterBase();
    virtual ~ImporterBase();

    // A derived class may reimplement this
    virtual bool import(const QString &source,const QString &newName,QString *results);

    // A derived class must implement these
    virtual const ImportManager::formatInfo *formatInfo() = 0;
    virtual bool doImport(QFile &f,Episode *episode,Sprites *sprites,MapList *maplist,QString *results) = 0;
};


#endif							// IMPORTERBASE_H
