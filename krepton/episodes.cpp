////////////////////////////////////////////////////////////////////////////
//  
//  KReptonEd - editor for Repton levels for KDE
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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <kapplication.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <qptrlist.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtstream.h>
#include <qregexp.h>

#include "krepton.h"
#include "map.h"

#include "episodes.h"


const QString Episode::loadMaps(MapList *maps) const
{
	maps->setAutoDelete(true);
	maps->clear();					// clear existing maps
	maps->setAutoDelete(false);

	QString path = getFilePath(QString::null);	// containing directory
	QDir dir(path);					// just for safety
	if (!dir.exists())
	{
		return (QString("%1, %2").arg(strerror(ENOTDIR)).arg(path));
	}

	dir.setFilter(QDir::Files);			// enumerate map files
	dir.setNameFilter("map*");
	dir.setSorting(QDir::Name);

	QString status = QString::null;
	const QFileInfoList *list = dir.entryInfoList();
	QFileInfoListIterator it(*list);
	for (const QFileInfo *fi; (fi = it.current())!=NULL; ++it)
	{
		Map *m = new Map(path+"/"+fi->fileName());
		status = m->loadStatus();
		if (!status.isNull()) break;
		maps->append(m);
	}

	return (status);
}


static void removeMapFiles(const QString& path)
{
	QDir dir(path);
	if (!dir.exists()) return;			// equivalent to success

	dir.setFilter(QDir::Files);
	dir.setNameFilter("map*");
	dir.setSorting(QDir::Name);
	const QFileInfoList *list = dir.entryInfoList();
	QFileInfoListIterator it(*list);
	for (const QFileInfo *fi; (fi = it.current())!=NULL; ++it)
	{
		kdDebug(0) << k_funcinfo << "remove old " << fi->absFilePath() << endl;
		dir.remove(fi->fileName());
	}
}


bool Episode::saveInfoAndMaps(const MapList *maps) const
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;

	QString path = getFilePath(QString::null);	// containing directory
	if (!QDir(path).exists())
	{
		if (!KStandardDirs::makeDir(path))
		{
			reportError("Cannot create directory '%1'",path);
			return (false);
		}
	}

	path = getFilePath("info");			// information file
	QFile f(path);
	if (!f.open(IO_WriteOnly))
	{
		reportError("Cannot write to file '%1'",path);
		return (false);
	}

	QTextStream t(&f);
	t << name << '\n';
	t << maps->count() << '\n';
	f.close();

	removeMapFiles(path);				// clean up old maps
	MapListIterator mi(*maps);
	const Map *mm;
	for (int i = 1; (mm = mi.current())!=NULL; ++mi,++i)
	{						// save all maps
		const QString p1 = getFilePath(QString("map%1").arg(i));
		if (!mm->save(p1)) return (false);
	}

	return (true);
}



bool Episode::removeFiles() const
{
	kdDebug(0) << k_funcinfo << "path='" << path << "'" << endl;

	QDir dir(path);
	if (!dir.exists())
	{
		reportError("Directory '%1' does not exist");
		return (true);				// equivalent to success
	}

	QString dirname = dir.dirName();
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);

	const QFileInfoList *list = dir.entryInfoList();
	QFileInfoListIterator it(*list);
	for (const QFileInfo *fi; (fi = it.current())!=NULL; ++it)
	{
		if (!dir.remove(fi->fileName()))
		{
			reportError("Cannot delete file '%1'",dir.absFilePath(fi->fileName()));
			break;
		}
	}

	dir.cdUp();
	if (!dir.rmdir(path))
	{
		reportError("Cannot delete directory '%1'",path);
		return (false);
	}

	return (true);
}


QString Episode::sanitisedName(const QString &name)
{
	QString s1 = name.simplifyWhiteSpace();
	QString t = "";

	QChar ch;
	for (int i = 0; !((ch = s1.at(i)).isNull()); ++i)
	{
		if (ch.latin1()==0 || !ch.isLetterOrNumber()) ch = '_';
		t += ch.lower();
	}

        return (t);
}




Episode::Episode(const QString n,bool g,const QString p)
{
	name = n;
	global = g;
	path = p;
	path.replace(QRegExp("/+$"),QString(""));
}


const QString Episode::getFilePath(const QString file) const
{
	return (path+"/"+file);
}


QString Episode::savePath(const QString &name)
{
	return (KGlobal::dirs()->saveLocation("episodes",QString::null,false)+
		sanitisedName(name)+"/");
}





EpisodeList::EpisodeList() : QPtrList<Episode>()
{
	const QString localdir = QDir(KGlobal::dirs()->localkdedir()).canonicalPath()+"/";
	kdDebug(0) << k_funcinfo << "local='" << localdir << "'" << endl;

	QStringList dirs = KGlobal::dirs()->findDirs("episodes","");
	for (QStringList::Iterator di = dirs.begin(); di!=dirs.end(); ++di)
	{
		QString di1 = *di;

		kdDebug(0) << k_funcinfo << "dir='" << di1 << "'" << endl;
		if (di1==QString::null) continue;
		di1 = QDir(di1).canonicalPath();

		QDir dir(di1);
		if (!dir.exists())
		{
			reportError("Episode directory '%1' not found",di1);
			continue;
		}
 
		dir.setFilter(QDir::Dirs);
		dir.setSorting(QDir::Name);
		const QFileInfoList *list = dir.entryInfoList();
		QFileInfoListIterator it(*list);
		for (const QFileInfo *fi; (fi = it.current())!=NULL; ++it)
		{
			if (fi->fileName().startsWith(".")) continue;

			if (!dir.cd(fi->fileName()))
			{
				reportError("Cannot access directory '%1'",
				      dir.absFilePath(fi->fileName()));
				continue;
			}

			QString dirname = dir.absPath();
			QString filename = dirname+"/info";
			QFile f(filename);
			dir.cdUp();

			if (!f.open(IO_ReadOnly))
			{
				reportError("Cannot read information file '%1'",filename);
				continue;
			}

			QTextStream t(&f);
			QString name;
			name = t.readLine().stripWhiteSpace();
			f.close();

			bool global = !di1.startsWith(localdir);
			append(new Episode(name,global,dirname));
		}
	}

	if (isEmpty())
	{
		QString msg;
		QStringList dirs = KGlobal::dirs()->findDirs("episodes","");

		msg = "No episodes could be found in any of these locations:";
		msg += "<br><ul><li>"+dirs.join("</li><li>")+"</li></ul>";
		msg += "Is the application properly installed?";

		reportError(msg,QString::null,false);
	}

	setAutoDelete(true);
	sort();
	kdDebug(0) << k_funcinfo << "done" << endl;
}



EpisodeList *EpisodeList::list()
{

//	if (episodelist==NULL) episodelist = new EpisodeList();
//	return (episodelist);
	static EpisodeList *el = new EpisodeList();
	return (el);
}


const Episode *EpisodeList::find(const QString &name)
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;
	for (const Episode *e = first(); e!=NULL; e = next())
	{
		if (name.lower()==e->getName().lower()) return (e);
	}

	return (NULL);
}

void EpisodeList::add(const Episode *e)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;
	if (findRef(e)==-1) inSort(e);
}


void EpisodeList::remove(const Episode *e,bool noDelete)
{
	kdDebug(0) << k_funcinfo << "name='" << e->getName() << "'" << endl;

        bool deleteState = autoDelete();
        setAutoDelete(!noDelete);
	removeRef(e);
        setAutoDelete(deleteState);
}


bool EpisodeList::anyUser() const
{
	QPtrListIterator<Episode> ei(*this);
	for (const Episode *e; (e = ei.current())!=NULL; ++ei)
	{
		if (!e->isGlobal()) return (true);
	}
	return (false);
}

bool EpisodeList::any() const
{
	return (count()>0);
}

int EpisodeList::compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2)
{
	const QString n1 = ((const Episode *) item1)->getName();
	const QString n2 = ((const Episode *) item2)->getName();
//	kdDebug(0) << k_funcinfo << "1='" << n1 << "' 2='" << n2 << "'" << endl;

	if (n1=="blank") return (+1);			// always force to end
	if (n2=="blank") return (-1);
	return (n1.lower().compare(n2.lower()));
}
