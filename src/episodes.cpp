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

#include "episodes.h"

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <kmessagebox.h>

#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qalgorithms.h>
#include <qstandardpaths.h>

#include "krepton.h"
#include "map.h"
#include "resourcelist.h"


const QString Episode::loadMaps(MapList *maps) const
{
	qDeleteAll(*maps);				// clear existing maps
	maps->clear();

	QString path = getFilePath("");			// containing directory
	QDir dir(path);					// just for safety
	if (!dir.exists())
	{
		return (QString("%1, %2").arg(strerror(ENOTDIR)).arg(path));
	}

	dir.setFilter(QDir::Files);			// enumerate map files
	dir.setNameFilters(QStringList("map*"));
	dir.setSorting(QDir::Name);

	QString status = "";
	const QFileInfoList list = dir.entryInfoList();
        for (QFileInfoList::const_iterator it = list.constBegin();
             it!=list.constEnd(); ++it)
        {
                QFileInfo fi = (*it);
		Map *m = new Map(path+"/"+fi.fileName());
		status = m->loadStatus();
		if (!status.isEmpty()) break;
		maps->append(m);
	}

	return (status);
}


static void removeMapFiles(const QString& path)
{
	QDir dir(path);
	if (!dir.exists()) return;			// equivalent to success

	dir.setFilter(QDir::Files);
	dir.setNameFilters(QStringList("map*"));
	dir.setSorting(QDir::Name);
	const QFileInfoList list = dir.entryInfoList();
        for (QFileInfoList::const_iterator it = list.constBegin();
             it!=list.constEnd(); ++it)
        {
                QFileInfo fi = (*it);
		qDebug() << "remove old " << fi.absoluteFilePath();
		dir.remove(fi.fileName());
	}
}


bool Episode::saveInfoAndMaps(const MapList *maps) const
{
	qDebug() << "name='" << name << "'";

	QString path = getFilePath("");			// containing directory
	if (!QDir(path).exists())
	{
		if (!QDir::root().mkpath(path))
		{
			reportError(ki18n("Cannot create directory '%1'"), path);
			return (false);
		}
	}

	path = getFilePath("info");			// information file
	QFile f(path);
	if (!f.open(QIODevice::WriteOnly))
	{
		reportError(ki18n("Cannot write to file '%1'"), path);
		return (false);
	}

	QTextStream t(&f);
	t << name << '\n';
	t << maps->count() << '\n';
	f.close();

	path = getFilePath("");				// containing directory
	removeMapFiles(path);				// clean up old maps
        int i = 0;
        for (MapList::const_iterator it = maps->constBegin();
		it!=maps->constEnd(); ++it, ++i)
	{						// save all maps
		const Map *mm = (*it);
                const QString p1 = getFilePath(QString("map%1").arg(i, 3, 10, QLatin1Char('0')));
		if (!mm->save(p1)) return (false);
        }

	return (true);
}



bool Episode::removeFiles() const
{
	qDebug() << "path='" << filePath << "'";

	QDir dir(filePath);
	if (!dir.exists())
	{
		reportError(ki18n("Directory '%1' does not exist"), filePath);
		return (true);				// equivalent to success
	}

	QString dirname = dir.dirName();
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);

	// TODO: only need names listed here, no sorting
	// or even just recursive delete
	const QFileInfoList list = dir.entryInfoList();
        for (QFileInfoList::const_iterator it = list.constBegin();
             it!=list.constEnd(); ++it)
        {
                QFileInfo fi = (*it);
		if (!dir.remove(fi.fileName()))
		{
			reportError(ki18n("Cannot delete file '%1'"), dir.absoluteFilePath(fi.fileName()));
			break;
		}
	}

	dir.cdUp();
	if (!dir.rmdir(filePath))
	{
		reportError(ki18n("Cannot delete directory '%1'"), filePath);
		return (false);
	}

	return (true);
}


QString Episode::sanitisedName(const QString &name)
{
	QString s1 = name.simplified();
	QString t = "";

	for (int i = 0; i<s1.length(); ++i)
	{
                QChar ch = s1.at(i);
		// TODO: substitute with regexp and character class
		if (ch.toLatin1()==0 || !ch.isLetterOrNumber()) ch = '_';
		t += ch.toLower();
	}

        return (t);
}


Episode::Episode(const QString n,bool g,const QString p)
{
	name = n;
	global = g;
	filePath = p;
	filePath.replace(QRegExp("/+$"),QString(""));
}


const QString Episode::getFilePath(const QString file) const
{
	return (filePath+"/"+file);
}


QString Episode::savePath(const QString &name)
{
	return (QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
		+"/episodes/"
		+sanitisedName(name)+"/");
}


bool episodeLessThan(const Episode *item1,const Episode *item2)
{
	const QString n1 = item1->getName();
	const QString n2 = item2->getName();
	//qDebug() << n1 << "-" << n2;

	if (n1=="blank") return (false);		// always force to end
	if (n2=="blank") return (true);
	return (n1.toLower()<n2.toLower());
}


EpisodeList::EpisodeList()
{
	const ResourceMap episodes = ResourceList::findResources("episodes");
	if (episodes.isEmpty())
	{
		reportError(ki18n("<qt>No episodes could be found.<br>Is the application properly installed?"),
			    QString(), false, true);
		return;
	}

	for (ResourceMap::const_iterator it = episodes.constBegin(); it!=episodes.constEnd(); ++it)
	{
		const QString &path = it.key();
		const QString &name = it.value();
		append(new Episode(name, ResourceList::isGlobal(path), path));
	}

	std::sort(begin(),end(),&episodeLessThan);		// alphabetical by names
}


EpisodeList *EpisodeList::list()
{
	static EpisodeList *el = new EpisodeList();
	return (el);
}


const Episode *EpisodeList::find(const QString &name)
{
	qDebug() << "name='" << name << "'";

        for (EpisodeList::const_iterator it = constBegin();
		it!=constEnd(); ++it)
	{
		const Episode *e = (*it);
		if (QString::compare(name, e->getName(), Qt::CaseInsensitive)==0) return (e);
	}

	return (NULL);
}

void EpisodeList::add(const Episode *e)
{
	qDebug() << "name" << e->getName();

	const Episode *old = find(e->getName());	// name already exists?
	if (old!=NULL) removeOne(old);			// yes, remove the old one

        append(e);
	std::sort(begin(), end(), &episodeLessThan);	// resort after insertion
}


void EpisodeList::remove(const Episode *e,bool noDelete)
{
	qDebug() << "name='" << e->getName() << "'";
        if (!noDelete) delete e;
	removeOne(e);
}


bool EpisodeList::anyUser() const
{
        for (EpisodeList::const_iterator it = constBegin();
		it!=constEnd(); ++it)
	{
		const Episode *e = (*it);
		if (!e->isGlobal()) return (true);
	}
	return (false);
}

bool EpisodeList::any() const
{
	return (count()>0);
}
