//////////////////////////////////////////////////////////////////////////
//									//
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

#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kmessagebox.h>

#include "krepton.h"
#include "mainwindow.h"
#include "version.h"


int main(int argc,char *argv[])
{
    KAboutData aboutData("krepton",				// appName
                         NULL,					// catalogName
                         ki18n("KRepton"),			// programName
#ifdef VCS_HAVE_VERSION
                         ( VERSION " (" VCS_TYPE_STRING " " VCS_REVISION_STRING ")" ),
#else
                         VERSION,				// version
#endif
                         ki18n("The Repton game for KDE"),
                         KAboutData::License_GPL,
                         ki18n("Copyright (C) 1998 Sandro Sigala, 2003-2010 Jonathan Marten"),
                         KLocalizedString(),			// text
                         "http://www.keelhaul.me.uk/krepton/",	// homePageAddress
                        "jjm@keelhaul.me.uk");			// bugsEmailAddress
    aboutData.addAuthor(ki18n("Jonathan Marten"),
                        ki18n("KDE3 and KDE4 conversion"),
                        "jjm@keelhaul.me.uk",
                        "http://www.keelhaul.me.uk");
    aboutData.addAuthor(ki18n("Sandro Sigala"),
                         ki18n("Original author"),
                        "ssigala@globalnet.it");
    aboutData.addCredit(ki18n("Jasper Renow-Clarke"),
                        ki18n("BBC format and maps"),
                        "cm5hdjjr@bs41.staffs.ac.uk");
    aboutData.addCredit(ki18n("George Russell"),
                        ki18n("Feature suggestions"),
                        "george.russell@clara.net");
    aboutData.addCredit(ki18n("Stairway to Hell contributors"),
                        ki18n("Compatibility questions"),
                        "",
                        "http://www.stairwaytohell.com");

    //aboutData.setProgramIconName("krepton");
    //aboutData.setProgramIconName("krepton");

    KCmdLineOptions opts;
    // TODO: options for mode, start game or editor
    opts.add("l <episode>", ki18n("Name of episode to load"));
    KCmdLineArgs::addCmdLineOptions(opts);

    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QString name;					// '-l' -> load episode
    if (args->isSet("l")) name = args->getOption("l");
    args->clear();

    srand(time(NULL));

    KGlobal::dirs()->addResourceType("episodes", "appdata", "episodes");
    KGlobal::dirs()->addResourceType("graphics", "appdata", "pics");
    KGlobal::dirs()->addResourceType("sound", "appdata", "sounds");

    MainWindow *w = new MainWindow(NULL);
    w->show();
    if (!name.isEmpty()) w->loadGame(name);

    return (app.exec());
}  


void reportError(const KLocalizedString &message,
                 const QString &filename,
		 bool system, bool fatal)
{
        QString s = "<qt>";
        if (!filename.isEmpty())
        {
                s += message.subs(QString("<filename>%1</filename>").arg(filename)).toString();
        }
        else
        {
                s += message.toString();
        }
#ifdef HAVE_STRERROR
	if (system)
	{
		s += "<p>The error reported was: ";
		s += strerror(errno);
	}
#endif
	if (fatal)
	{
		KMessageBox::error(NULL, s);
		kapp->exit(1);
	}
	else KMessageBox::sorry(NULL, s);
}
