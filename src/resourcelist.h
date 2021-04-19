////////////////////////////////////////////////////// -*- mode:c++; -*- ///
//  
//  KReptonEd - editor for Repton levels for KDE
//  
//  Copyright (c) 2021 Jonathan Marten <jjm@keelhaul.demon.co.uk>
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

#ifndef RESOURCELIST_H
#define RESOURCELIST_H

#include <qmap.h>


/**
 * A collection of returned resources.  The @c key() is the full path
 * to the resource, the last component of which should be the internal
 * name.  The @c value() is the user-visible name.
 **/
typedef QMap<QString, QString> ResourceMap;

namespace ResourceList
{
    /**
     * Find all resources of the specified @p type, which should be
     * the name of a resource directory wich is expected to be under
     * @c QStandardPaths::AppDataLocation.
     **/
    ResourceMap findResources(const QByteArray &type);

    /**
     * Get the internal name from a resource pathname.
     *
     * @p resPath The full resource path name, as returned by @c findResources().
     * @return the internal name.
     **/
    QString internalName(const QString &resPath);


    /**
     * Check whether the specified resource is global, i.e. it is not
     * saved in the user's local data directory.
     *
     * @p resPath The full resource path name, as returned by @c findResources().
     * @return @c true if the resource is global.
     **/
    bool isGlobal(const QString &resPath);
}

#endif							// !RESOURCELIST_H
