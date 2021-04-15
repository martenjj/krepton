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

#include <qapplication.h>
#include <qcommandlineparser.h>

#include <kaboutdata.h>
#include <kcrash.h>
#include <kmessagebox.h>

#include "krepton.h"
#include "mainwindow.h"
#include "version.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KAboutData aboutData("krepton",				// componentName
                         i18n("KRepton"),			// displayName
#ifdef VCS_HAVE_VERSION
                         ( VERSION " (" VCS_TYPE_STRING " " VCS_REVISION_STRING ")" ),
#else
                         VERSION,				// version
#endif
                         i18n("The Repton game for KDE"),
                         KAboutLicense::GPL,
                         i18n("Copyright (C) 1998 Sandro Sigala, 2003-2010 Jonathan Marten"),
                         "",					// otherText
                         "http://www.keelhaul.me.uk/krepton/",	// homePageAddress
                        "jjm@keelhaul.me.uk");			// bugsEmailAddress

    aboutData.addAuthor(i18n("Jonathan Marten"),
                        i18n("KDE3 and KDE4 conversion"),
                        "jjm@keelhaul.me.uk",
                        "http://www.keelhaul.me.uk");
    aboutData.addAuthor(i18n("Sandro Sigala"),
                         i18n("Original author"),
                        "ssigala@globalnet.it");
    aboutData.addCredit(i18n("Jasper Renow-Clarke"),
                        i18n("BBC format and maps"),
                        "cm5hdjjr@bs41.staffs.ac.uk");
    aboutData.addCredit(i18n("George Russell"),
                        i18n("Feature suggestions"),
                        "george.russell@clara.net");
    aboutData.addCredit(i18n("Stairway to Hell contributors"),
                        i18n("Compatibility questions"),
                        "",
                        "http://www.stairwaytohell.com");

    KAboutData::setApplicationData(aboutData);
    KCrash::setDrKonqiEnabled(true);

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());

    parser.addPositionalArgument("episode", i18n("Name of episode to load"), i18n("[episode]"));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    const QString name = parser.positionalArguments().value(0);

    srand(time(NULL));

//     KGlobal::dirs()->addResourceType("episodes", "appdata", "episodes");
//     KGlobal::dirs()->addResourceType("graphics", "appdata", "pics");
//     KGlobal::dirs()->addResourceType("appsound", "appdata", "sounds");

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
                qApp->exit(1);
	}
	else KMessageBox::sorry(NULL, s);
}
