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
#include <kfiledialog.h>
#include <kactivelabel.h>
#include <ktextedit.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qfile.h>
#include <qfileinfo.h>
//#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcursor.h>

//#include "gamemanager.h"

#include "episodes.h"
#include "importmanager.h"
#include "importerbase.h"

#include "importwizard.h"
#include "importwizard.moc"


//////////////////////////////////////////////////////////////////////////
//									//
//  Data for quick testing						//
//									//
//////////////////////////////////////////////////////////////////////////

#undef  TESTING
#define TEST_FORMAT		0
#define TEST_SOURCE		"/usr/home/ws/jjm/krepton-4.1/krepton/test/MFoot/JULIE"
#define TEST_NAME		"JULIE"

//////////////////////////////////////////////////////////////////////////
//									//
//  Text strings							//
//									//
//////////////////////////////////////////////////////////////////////////

static const QString page1caption = i18n("Select Import Format");
static const QString page2caption = i18n("Specify File To Import");
static const QString page3caption = i18n("New Episode Name");
static const QString page4caption = i18n("Ready To Import");
static const QString page5caption = i18n("Finished");

static const QString page1text = i18n("<qt><p>Repton games in a variety of formats can be imported. \
An imported game will be saved and can be selected to play or edit in the same way as any other.");

static const QString page2text = i18n("<qt><p>Importing a %1 data file.\
<p>\
Specify the file to be imported.");

static const QString page3text = i18n("<qt><p>Specify a name for the imported episode.");

static const QString page4text = i18n("<qt><p>Use <b>Next</b> to start importing the episode as specified.");

static const QString page5text = i18n("<qt><p>Use <b>Finish</b> to exit the wizard.<p>To immediately load the new episode, select the check box below.");

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor						//
//									//
//////////////////////////////////////////////////////////////////////////

ImportWizard::ImportWizard(const QString &title,QWidget *parent)
	: KWizard(parent)
{
	kdDebug(0) << k_funcinfo << endl;

        format = NULL;					// none selected yet
        manager = ImportManager::self();		// we'll use this a lot
        importedOk = false;

	setCaption(title);
	setupPage1();
	setupPage2();
	setupPage3();
	setupPage4();
	setupPage5();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Moving between pages						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::showPage(QWidget *page)
{
	KWizard::showPage(page);

	if (page==page1)
	{
#ifdef TESTING
		page1list->setCurrentItem(TEST_FORMAT);
#endif
		slotPage1FormatSelected();
	}
	else if (page==page2)
	{
        	if (format!=NULL)
		{
			page2info->setText(page2text.arg(format->name));
		}
#ifdef TESTING
		page2source->setURL(TEST_SOURCE);
#endif
		slotPage2SourceSelected();
	}
	else if (page==page3)
	{
		if (format!=NULL)
                {
			const KURL u = page2source->url();
			if (!u.isEmpty() && page3name->text().isEmpty())
			{
				QString f = u.fileName().section('.',0,0);
				page3name->setText(f);
			}
		}
#ifdef TESTING
                page3name->setText(TEST_NAME);
		page3over->setChecked(true);
#endif
		slotPage3NameChanged();
	}
	else if (page==page4)
	{
		if (format!=NULL)
                {
			QString t = "<qt><dl>";
			t += i18n("<dt>Format:<dd><b>%1</b>").arg(format->name);
			t += i18n("<dt>Source file:<dd><b>%1</b>").arg(page2source->url());
			t += i18n("<dt>New episode name:<dd><b>%1</b>").arg(page3name->text());
                        t += "</dl>";
                        page4disp->setText(t);
                }
        }
        else if (page==page5)
        {
		setCursor(QCursor(Qt::WaitCursor));

		ImporterBase *importer = ImportManager::self()->createImporter(format->key);
		if (importer==NULL)
		{
			reportError(i18n("Cannot create importer for '%1'"),format->key,false);
			finishButton()->setEnabled(false);
			page5disp->setText(i18n("Error while importing"));
		}
		else
		{
			QString status;
                        episodeName = page3name->text();
                        if (!importer->import(page2source->url(),episodeName,&status))
                        {
				if (status.isEmpty()) status = i18n("Import failed");
				finishButton()->setEnabled(false);
				page5load->setChecked(false);
				page5load->setEnabled(false);
                        }
			else
			{
				if (status.isEmpty()) status = i18n("Import complete");
				importedOk = true;
			}
			page5disp->setText(status);
                }
		unsetCursor();
        }
}


void ImportWizard::next()
{
	bool ok = true;

	if (currentPage()==page2)
	{
		const QString src = page2source->url();
		ok = (!src.isEmpty());
		if (ok)
		{
			QFileInfo fi(src);
			if (!fi.exists())
			{
				reportError(i18n("The file <b>%1</b><br>does not exist."),fi.absFilePath(),false);
				ok = false;
			}

			if (ok && fi.isRelative())
			{
				fi.convertToAbs();
				page2source->setURL(fi.filePath());
			}

			if (ok)
			{
				KConfig *conf = KGlobal::config();
				conf->setGroup("Importer");
				conf->writePathEntry("LastLocation",page2source->fileDialog()->baseURL().url());
                        }
		}
	}
	else if (currentPage()==page3)
	{
		const QString name = page3name->text();
		ok = (!name.isEmpty());
		if (ok)
		{
			const Episode *e = EpisodeList::list()->find(name);
                        if (e!=NULL)
                        {
				if (e->isGlobal())
                                {
					reportError(i18n("A global episode named <b>%1</b> already exists."),name,false);
                                        ok = false;
                                }
                                else if (!page3over->isChecked())
                                {
					reportError(i18n("A user episode named <b>%1</b> already exists.<br>Select the \"Overwrite\" check box if required."),name,false);
                                        ok = false;
                                }
			}
                }
	}

	if (ok) KWizard::next();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 1:  list of formats						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage1()
{
	page1 = new QWidget(this);

	QHBoxLayout *l = new QHBoxLayout(page1,KDialog::marginHint(),KDialog::spacingHint());

	page1list = new QListBox(page1);
	page1list->setSelectionMode(QListBox::Single);
	page1list->setHScrollBarMode(QScrollView::AlwaysOff);
	page1list->setVScrollBarMode(QScrollView::AlwaysOn);
	page1list->setMinimumHeight(80);
	connect(page1list,SIGNAL(selectionChanged()),this,SLOT(slotPage1FormatSelected()));
	l->addWidget(page1list);

	for (const ImportManager::formatInfo *it = manager->firstInfo(); it!=NULL; it = manager->nextInfo())
	{
		QString s = it->name;
		page1list->insertItem(s);
	}

	page1list->setMinimumWidth(page1list->maxItemWidth()+page1list->verticalScrollBar()->width()+20);

	page1info = new KActiveLabel(page1text+"<p>"+i18n("Select a format for more information about it."),page1);
	l->addWidget(page1info,1);

	page1->setMinimumHeight(240);
	addPage(page1,page1caption);
	setHelpEnabled(page1,false);
}


void ImportWizard::slotPage1FormatSelected()
{
    QString t = page1text;

    QListBoxItem *item = page1list->selectedItem();
    QString fmtName = (item==NULL ? QString::null : item->text());

    format = manager->findNamed(fmtName);
    if (format!=NULL)
    {
        t += "<p><hr>";
        if (!format->url.isNull())
        {
            t += "<p>"+i18n("See <a href=\"%1\">%2</a> for more information on this format.").arg(format->url,format->url);
        }

        if (!format->notes.isNull())
        {
            t += "<p>"+format->notes;
        }
        else
        {
            if (format->url.isNull())
            {
                t += "<p>"+i18n("No further information is available on this format.");
            }
        }
        page1info->setText(t);
    }

    setNextEnabled(page1,format!=NULL);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 2:  source location						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage2()
{
	page2 = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(page2,KDialog::marginHint(),KDialog::spacingHint());

	page2info = new QLabel(page2text.arg(""),page2);
	l->addWidget(page2info,1,Qt::AlignTop);

	page2source = new KURLRequester(page2);
	page2source->setMode(KFile::File+KFile::ExistingOnly+KFile::LocalOnly);

	page2source->fileDialog()->setCaption(page2caption);

        KConfig *conf = KGlobal::config();
        conf->setGroup("Importer");
	QString lastloc = conf->readPathEntry("LastLocation");
	if (!lastloc.isNull()) page2source->fileDialog()->setURL(lastloc);

	connect(page2source,SIGNAL(textChanged(const QString &)),this,SLOT(slotPage2SourceSelected()));

	QLabel *lab = new QLabel(i18n("Source file:"),page2);
	lab->setBuddy(page2source);
	l->addWidget(lab,1,Qt::AlignBottom);
	l->addWidget(page2source,0,Qt::AlignTop);

	addPage(page2,page2caption);
	setHelpEnabled(page2,false);
	nextButton()->setDefault(true);
}


void ImportWizard::slotPage2SourceSelected()
{
	kdDebug(0) << k_funcinfo << " url=" << page2source->url() << endl;
	setNextEnabled(page2,!page2source->url().isEmpty());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 3:  episode name						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage3()
{
	page3 = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(page3,KDialog::marginHint(),KDialog::spacingHint());

	KActiveLabel *info = new KActiveLabel(page3text,page3);
	l->addWidget(info,1,Qt::AlignTop);

	page3name = new QLineEdit(page3);
	connect(page3name,SIGNAL(textChanged(const QString &)),this,SLOT(slotPage3NameChanged()));

	QLabel *lab = new QLabel(i18n("Episode name:"),page3);
	lab->setBuddy(page3name);
	l->addWidget(lab,1,Qt::AlignBottom);
	l->addWidget(page3name,0,Qt::AlignTop);

        page3over = new QCheckBox(i18n("Overwrite any existing episode"),page3);
        l->addWidget(page3over,0,Qt::AlignLeft);

	addPage(page3,page3caption);
	setHelpEnabled(page3,false);
	nextButton()->setDefault(true);
}


void ImportWizard::slotPage3NameChanged()
{
	const QString text = page3name->text();
	setNextEnabled(page3,!text.isEmpty());
}


//////////////////////////////////////////////////////////////////////////
//									//
//  Page 4:  ready to import						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage4()
{
	page4 = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(page4,KDialog::marginHint(),KDialog::spacingHint());

	QLabel *info = new QLabel(page4text,page4);
	l->addWidget(info,1,Qt::AlignTop);

        page4disp = new KTextEdit(page4);
        page4disp->setReadOnly(true);
        page4disp->setText("?");
        l->addWidget(page4disp,1);

	addPage(page4,page4caption);
	setHelpEnabled(page4,false);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 5:  finished							//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage5()
{
	page5 = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(page5,KDialog::marginHint(),KDialog::spacingHint());

	QLabel *info = new QLabel(page5text,page5);
	l->addWidget(info,1,Qt::AlignTop);

        page5load = new QCheckBox(i18n("Load the new episode"),page5);
        l->addWidget(page5load,0,Qt::AlignLeft);

        l->addSpacing(KDialog::spacingHint());

        page5disp = new KTextEdit(page5);
        page5disp->setReadOnly(true);
        page5disp->setText("?");
        l->addWidget(page5disp,1);

	addPage(page5,page5caption);
	setHelpEnabled(page5,false);
	setFinishEnabled(page5,true);
	finishButton()->setDefault(true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Returned results							//
//									//
////////////////////////////////////////////////////////////////////////////

QString ImportWizard::newEpisodeToLoad() const
{
	return ((importedOk && page5load->isChecked()) ? episodeName : QString::null);
}
