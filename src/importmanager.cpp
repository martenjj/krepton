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
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>
#include <kurlrequester.h>
#include <kactivelabel.h>

#include <qlabel.h>
#include <qlayout.h>
#include <q3listbox.h>
#include <qwidget.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3PtrList>

#include "importmanager.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Instance and access							//
//									//
//////////////////////////////////////////////////////////////////////////

static ImportManager *instance = NULL;

ImportManager *ImportManager::self()
{
	if (instance==NULL) instance = new ImportManager();
	return (instance);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor						//
//									//
//////////////////////////////////////////////////////////////////////////

ImportManager::ImportManager()
{
	formats = new Q3PtrList<ImportManager::formatInfo>;
}


ImportManager::~ImportManager()
{
	delete formats;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Register a new engine, done for each at initialisation time		//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportManager::add(const ImportManager::formatInfo *inf)
{
	kDebug() << "add '" << inf->key << "' = '" << inf->name << "'";
	formats->append(inf);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Create an importer implementation object				//
//									//
//////////////////////////////////////////////////////////////////////////

ImporterBase *ImportManager::createImporter(const QString &key)
{
	kDebug() << "for '" << key << "'";

	const ImportManager::formatInfo *inf = ImportManager::findInfo(key);
	if (inf==NULL) return (NULL);
	return ((inf->create)());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Searching and iterating over the list				//
//									//
//////////////////////////////////////////////////////////////////////////

const ImportManager::formatInfo *ImportManager::firstInfo() const
{
	return (formats->first());
}


const ImportManager::formatInfo *ImportManager::nextInfo() const
{
	return (formats->next());
}


const ImportManager::formatInfo *ImportManager::findInfo(const QString &key) const
{
	for (const ImportManager::formatInfo *it = formats->first(); it!=NULL; it = formats->next())
	{
		if (key==it->key) return (it);
	}
	return (NULL);
}


const ImportManager::formatInfo *ImportManager::findNamed(const QString &name) const
{
	for (const ImportManager::formatInfo *it = formats->first(); it!=NULL; it = formats->next())
	{
		if (name==it->name) return (it);
	}
	return (NULL);
}
