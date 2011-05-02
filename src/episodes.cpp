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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <kapplication.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qalgorithms.h>

#include "krepton.h"
#include "map.h"

#include "episodes.h"


const QString Episode::loadMaps(MapList *maps) const
{
	qDeleteAll(*maps);				// clear existing maps
	maps->clear();

	QString path = getFilePath(QString::null);	// containing directory
	QDir dir(path);					// just for safety
	if (!dir.exists())
	{
		return (QString("%1, %2").arg(strerror(ENOTDIR)).arg(path));
	}

	dir.setFilter(QDir::Files);			// enumerate map files
	dir.setNameFilters(QStringList("map*"));
	dir.setSorting(QDir::Name);

	QString status = QString::null;
	const QFileInfoList list = dir.entryInfoList();
        for (QFileInfoList::const_iterator it = list.constBegin();
             it!=list.constEnd(); ++it)
        {
                QFileInfo fi = (*it);
		Map *m = new Map(path+"/"+fi.fileName());
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
	dir.setNameFilters(QStringList("map*"));
	dir.setSorting(QDir::Name);
	const QFileInfoList list = dir.entryInfoList();
        for (QFileInfoList::const_iterator it = list.constBegin();
             it!=list.constEnd(); ++it)
        {
                QFileInfo fi = (*it);
		kDebug() << "remove old " << fi.absoluteFilePath();
		dir.remove(fi.fileName());
	}
}


bool Episode::saveInfoAndMaps(const MapList *maps) const
{
	kDebug() << "name='" << name << "'";

	QString path = getFilePath(QString::null);	// containing directory
	if (!QDir(path).exists())
	{
		if (!KStandardDirs::makeDir(path))
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

	removeMapFiles(path);				// clean up old maps
        int i = 0;
        for (MapList::const_iterator it = maps->constBegin();
		it!=maps->constEnd(); ++it, ++i)
	{						// save all maps
		const Map *mm = (*it);
                const QString p1 = getFilePath(QString("map%1").arg(i));
		if (!mm->save(p1)) return (false);
        }

	return (true);
}



bool Episode::removeFiles() const
{
	kDebug() << "path='" << path << "'";

	QDir dir(path);
	if (!dir.exists())
	{
		reportError(ki18n("Directory '%1' does not exist"), path);
		return (true);				// equivalent to success
	}

	QString dirname = dir.dirName();
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);

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
	if (!dir.rmdir(path))
	{
		reportError(ki18n("Cannot delete directory '%1'"), path);
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
		if (ch.toLatin1()==0 || !ch.isLetterOrNumber()) ch = '_';
		t += ch.toLower();
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


bool episodeLessThan(const Episode *item1,const Episode *item2)
{
	const QString n1 = item1->getName();
	const QString n2 = item2->getName();
	//kDebug() << n1 << "-" << n2;

	if (n1=="blank") return (false);		// always force to end
	if (n2=="blank") return (true);
	return (n1.toLower()<n2.toLower());
}


EpisodeList::EpisodeList()
{
	const QString localdir = QDir(KGlobal::dirs()->localkdedir()).canonicalPath()+"/";
	kDebug() << "local='" << localdir << "'";

	QStringList dirs = KGlobal::dirs()->findDirs("episodes","");
	for (QStringList::Iterator di = dirs.begin(); di!=dirs.end(); ++di)
	{
		QString di1 = *di;

		kDebug() << "dir='" << di1 << "'";
		if (di1==QString::null) continue;
		di1 = QDir(di1).canonicalPath();

		QDir dir(di1);
		if (!dir.exists())
		{
			reportError(ki18n("Episode directory '%1' not found"), di1);
			continue;
		}
 
		dir.setFilter(QDir::Dirs);
		dir.setSorting(QDir::Name);
		const QFileInfoList list = dir.entryInfoList();
		for (QFileInfoList::const_iterator it = list.constBegin();
                     it!=list.constEnd(); ++it)
		{
                        QFileInfo fi = (*it);
			if (fi.fileName().startsWith(".")) continue;

			if (!dir.cd(fi.fileName()))
			{
				reportError(ki18n("Cannot access directory '%1'"),
				      dir.absoluteFilePath(fi.fileName()));
				continue;
			}

			QString dirname = dir.absolutePath();
			QString filename = dirname+"/info";
			QFile f(filename);
			dir.cdUp();

			if (!f.open(QIODevice::ReadOnly))
			{
				reportError(ki18n("Cannot read information file '%1'"), filename);
				continue;
			}

			QTextStream t(&f);
			QString name;
			name = t.readLine().trimmed();
			f.close();

			bool global = !di1.startsWith(localdir);
			append(new Episode(name,global,dirname));
		}
	}

	if (isEmpty())
	{
		QStringList dirs = KGlobal::dirs()->findDirs("episodes","");
                if (dirs.isEmpty()) dirs = QStringList(i18n("(none)"));

                // Not proper I18N here, but then the user should never see this message
		QString msg = "<qt>";
		msg = "No episodes could be found in any of these locations:";
		msg += "<br><br>&nbsp;&nbsp;<filename>"+dirs.join("</filename><br>&nbsp;&nbsp;<filename>")+"</filename><br><br>";
		msg += "Is the application properly installed?";

                KMessageBox::error(NULL, msg);
	}

	qSort(begin(),end(),&episodeLessThan);		// alphabetical by names
	kDebug() << "done";
}



EpisodeList *EpisodeList::list()
{
	static EpisodeList *el = new EpisodeList();
	return (el);
}


const Episode *EpisodeList::find(const QString &name)
{
	kDebug() << "name='" << name << "'";

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
	kDebug() << "name='" << e->getName() << "'";

        append(e);
	qSort(begin(),end(),&episodeLessThan);		// resort after insertion
}


void EpisodeList::remove(const Episode *e,bool noDelete)
{
	kDebug() << "name='" << e->getName() << "'";
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
