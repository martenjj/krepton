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

#ifndef IMPORTMANAGER_H
#define IMPORTMANAGER_H

#include <qstring.h>
#include <q3ptrlist.h>


class ImporterBase;


class ImportManager
{

public:
    static ImportManager *self();

    typedef ImporterBase * (*createFunction)();

    struct formatInfo
    {
        QString key;
        QString name;
        QString url;
        QString notes;
        unsigned int filesize;
        ImportManager::createFunction create;
    };

    ImporterBase *createImporter(const QString &key);

    void add(const ImportManager::formatInfo *info);

    const ImportManager::formatInfo *findNamed(const QString &name) const;
    const ImportManager::formatInfo *findInfo(const QString &key) const;
    const ImportManager::formatInfo *firstInfo() const;
    const ImportManager::formatInfo *nextInfo() const;

private:
    ImportManager();
    ~ImportManager();

    Q3PtrList<ImportManager::formatInfo> *formats;
};


#endif							// IMPORTMANAGER_H
