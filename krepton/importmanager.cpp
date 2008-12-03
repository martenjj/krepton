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

#include "config.h"

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kwizard.h>
#include <kdialog.h>
#include <kurlrequester.h>
//#include <kfiledialog.h>
//#include <kmessagebox.h>
#include <kactivelabel.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qwidget.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpushbutton.h>

//#include "gamemanager.h"

#include "importmanager.h"

//#include "importwizard.h"
//#include "importwizard.moc"


//////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////
////									//
////  Common text strings							//
////									//
////////////////////////////////////////////////////////////////////////////
//
//							// used for WAD and PWAD
//const QString GameManager::wadFilterText = i18n("*.WAD *.wad|Doom WAD Files (*.wad)\n"
//						"*.ZIP *.zip|Zip Archives (*.zip)\n"
//						"*|All Files");
//const QString GameManager::wadCaptionText = i18n("Select WAD");
//
//const QString GameManager::lmpFilterText = i18n("*.LMP *.lmp|Doom LMP Files (*.lmp)\n"
//						"*.ZIP *.zip|Zip Archives (*.zip)\n"
//						"*.EDM *.edm|EDGE Demo Files (*.edm)\n"
//						"*|All Files");
//const QString GameManager::lmpCaptionText = i18n("Select Demo");
//
////////////////////////////////////////////////////////////////////////////
////									//
////  Standard search paths						//
////									//
////////////////////////////////////////////////////////////////////////////
//
//static const QString execPaths = "/usr/games/bin:/usr/local/bin:/usr/bin";
//static const QString dataPaths = "/usr/share/games/doom-data:/usr/share/games/doom";

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
	formats = new QPtrList<ImportManager::formatInfo>;
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
	kdDebug(0) << k_funcinfo << "add '" << inf->key << "' = '" << inf->name << "'" << endl;
	formats->append(inf);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Create an importer implementation object				//
//									//
//////////////////////////////////////////////////////////////////////////

ImporterBase *ImportManager::createImporter(const QString &key)
{
	kdDebug(0) << k_funcinfo << "for '" << key << "'" << endl;

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

////////////////////////////////////////////////////////////////////////////
////									//
////  Searching for files							//
////									//
////////////////////////////////////////////////////////////////////////////
//
//const QString ImportManager::findExecFor(const ImportManager::formatInfo *g) const
//{
//	QString exe = QString::null;
//
//	const QStringList names = QStringList::split(':',g->execname);
//	for (QStringList::const_iterator it = names.begin(); it!=names.end(); ++it)
//	{						// try all possible names
//		if (!g->execpaths.isNull()) exe = KStandardDirs::findExe(*it,g->execpaths);
//		if (exe.isNull()) exe = KStandardDirs::findExe(*it,execPaths);
//		if (exe.isNull()) exe = KStandardDirs::findExe(*it);
//		if (!exe.isNull()) break;
//	}
//
//	kdDebug(0) << k_funcinfo << "for " << g->name << " = " << exe << endl;
//	return (exe);
//}
//
//
//const QString ImportManager::findDataFor(const ImportManager::formatInfo *g) const
//{
//	QString dir = QString::null;
//	QStringList dl;
//
//	if (!g->datapaths.isNull()) dl += QStringList::split(':',g->datapaths,true);
//	dl += QStringList::split(':',dataPaths,true);
//
//	for ( QStringList::ConstIterator it = dl.begin(); it!=dl.end(); ++it)
//	{						// try all possible locations
//		QDir d(*it);
//		if (!d.exists()) continue;
//
//		d.setFilter(QDir::Files+QDir::Readable);
//		d.setNameFilter("*.wad;*.WAD");
//		if (d.count()>0)			// look for any WAD files
//		{
//			dir = d.absPath();
//			break;
//		}
//	}
//
//	kdDebug(0) << k_funcinfo << "for " << g->name << " = " << dir << endl;
//	return (dir);
//}
