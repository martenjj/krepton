////////////////////////////////////////////////////////////////////////////
//  
//  KRepton - the classic Repton game for KDE
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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include "krepton.h"
#include "mainwindow.h"


// TODO: options for mode, start game or editor
static KCmdLineOptions options[] =
{
	{ "l <episode>", I18N_NOOP("Name of episode to load"), NULL },
	{ NULL, NULL, NULL }
};


int main(int argc,char *argv[])
{
	KAboutData aboutData("krepton",I18N_NOOP("KRepton"),
			     VERSION,
			     I18N_NOOP("The Repton game for KDE"),
			     KAboutData::License_GPL,
			     "(c) 1998, Sandro Sigala\n(c) 2003-2008, Jonathan Marten",0,
			     "http://www.keelhaul.me.uk/krepton/",
			     "jjm@keelhaul.me.uk");

	aboutData.addAuthor("Sandro Sigala",I18N_NOOP("Original author"),"ssigala@globalnet.it");
	aboutData.addAuthor("Jonathan Marten",I18N_NOOP("KDE 3 conversion"),"jjm@keelhaul.me.uk");
	aboutData.addCredit("Jasper Renow-Clarke",I18N_NOOP("BBC format and maps"),"cm5hdjjr@bs41.staffs.ac.uk");
	aboutData.addCredit("George Russell",I18N_NOOP("Feature suggestions"),"george.russell@clara.net");

	KCmdLineArgs::init(argc,argv,&aboutData);
	KCmdLineArgs::addCmdLineOptions(options);

	QCString name;					// '-l' -> load episode

	KApplication app;
 	if (!app.isRestored())
	{
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		if (args->isSet("l")) name = args->getOption("l");
		args->clear();
	}

	srand(time(NULL));

	KGlobal::dirs()->addResourceType("episodes",QString("share/apps/%1/episodes").arg(aboutData.appName()));
	KGlobal::dirs()->addResourceType("graphics",QString("share/apps/%1/graphics").arg(aboutData.appName()));
	KGlobal::dirs()->addResourceType("sound",QString("share/apps/%1/sounds").arg(aboutData.appName()));

	MainWindow *w = new MainWindow(NULL);
	w->show();
	if (!name.isNull()) w->loadGame(name);
	return (kapp->exec());
}  


void reportError(const QString &message,const QString &filename,
		 bool system,bool fatal)
{
	QString s = QString("<qt>%1").arg(message);
	if (filename!=QString::null) s = s.arg(filename);
	if (system)
	{
		s += "<p>The error reported was: ";
		s += strerror(errno);
	}

	if (fatal)
	{
		KMessageBox::error(0,s);
		kapp->exit(1);
	}
	else KMessageBox::sorry(0,s);
}
