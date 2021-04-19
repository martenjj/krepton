////////////////////////////////////////////////////////////////////////////
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

#include "episodes.h"


static QString localDataDir;


ResourceMap ResourceList::findResources(const QByteArray &type)
{
	qDebug() << "for type" << type;

	ResourceMap result;
	const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation,
							   type, QStandardPaths::LocateDirectory);

	if (localDataDir.isNull())			// not yet initialised
	{
		localDataDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+'/';
		qDebug() << "local data dir" << localDataDir;
	}

	for (const QString &d : dirs)
	{
		qDebug() << "resource dir" << d;

		QDir dir(d);
		if (!dir.exists())			// should never happen
		{
			qWarning() << "Resource directory" << d << "not found";
			continue;
		}

		dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
		dir.setSorting(QDir::Name);

		const QStringList list = dir.entryList();
		for (const QString &entry : list)
		{
			if (!dir.cd(entry))
			{
				reportError(ki18n("Cannot access directory '%1'"),
					    dir.absoluteFilePath(entry));
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
			const QString name = t.readLine().trimmed();
			qDebug() << name << "at" << dirname.toLocal8Bit().constData();
			result[dirname] = name;
			f.close();
		}
	}

	qDebug() << "listed" << result.count() << "resources";
	return (result);
}


QString ResourceList::internalName(const QString &resPath)
{
	const int idx = resPath.lastIndexOf('/');	// safe even if no '/'
	return (resPath.mid(idx+1));
}


bool ResourceList::isGlobal(const QString &resPath)
{
	return (!resPath.startsWith(localDataDir));
}
