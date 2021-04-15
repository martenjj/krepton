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

#include "importwizard.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qlistwidget.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qscrollbar.h>
#include <qfiledialog.h>
#include <qtextedit.h>

#include <kassistantdialog.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <kurlrequester.h>

#include <dialogbase.h>

#include "episodes.h"
#include "importmanager.h"
#include "importerbase.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Data for quick testing						//
//									//
//////////////////////////////////////////////////////////////////////////

#undef  TESTING
#define TEST_FORMAT		0
#define TEST_SOURCE		""
#define TEST_NAME		""

//////////////////////////////////////////////////////////////////////////
//									//
//  Text strings							//
//									//
//////////////////////////////////////////////////////////////////////////

static const KLocalizedString page1caption = ki18n("Select Import Format");
static const KLocalizedString page2caption = ki18n("Specify File To Import");
static const KLocalizedString page3caption = ki18n("New Episode Name");
static const KLocalizedString page4caption = ki18n("Ready To Import");
static const KLocalizedString page5caption = ki18n("Finished");

static const KLocalizedString page1text = ki18n("<qt><p>Repton games in a variety of formats can be imported. \
An imported game will be saved and can be selected to play or edit in the same way as any other.");

static const KLocalizedString page2text = ki18n("<qt><p>Importing a %1 data file.\
<p>\
Specify the file to be imported.");

static const KLocalizedString page3text = ki18n("<qt><p>Specify a name for the imported episode.");

static const KLocalizedString page4text = ki18n("<qt><p>Use <b>Import</b> to start importing the episode as specified.");

static const KLocalizedString page5text = ki18n("<qt><p>Use <b>Finish</b> to exit the assistant.<p>To immediately load the new episode, select the check box below.");

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor						//
//									//
//////////////////////////////////////////////////////////////////////////

ImportWizard::ImportWizard(const QString &title,QWidget *parent)
	: KAssistantDialog(parent)
{
	qDebug();

        format = NULL;					// none selected yet
        manager = ImportManager::self();		// we'll use this a lot
        importedOk = false;

	setWindowTitle(title);
	setupPage1();
	setupPage2();
	setupPage3();
	setupPage4();
	setupPage5();
	// TODO: port to KF5
// 	showButton(QDialogButtonBox::Help,false);

        connect(this,SIGNAL(currentPageChanged(KPageWidgetItem *,KPageWidgetItem *)),
                SLOT(slotShowPage(KPageWidgetItem *)));

        setMinimumSize(550, 300);
        slotShowPage(page1);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Moving between pages						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::slotShowPage(KPageWidgetItem *page)
{
	qDebug() << "new page" << page->name();

	if (page==page1)
	{
#ifdef TESTING
		page1list->setCurrentItem(TEST_FORMAT);
#endif
	// TODO: port to KF5
//                 setButtonText(KDialog::User2,i18n("Next"));
		slotPage1FormatSelected();
	}
	else if (page==page2)
	{
        	if (format!=NULL)
		{
			page2info->setText(page2text.subs(format->name).toString());
		}
#ifdef TESTING
		page2source->setURL(TEST_SOURCE);
#endif
	// TODO: port to KF5
//                 setButtonText(KDialog::User2,i18n("Next"));
		slotPage2SourceSelected();
	}
	else if (page==page3)
	{
		if (format!=NULL)
                {
			const QUrl u = page2source->url();
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
	// TODO: port to KF5
//                 setButtonText(KDialog::User2,i18n("Next"));
		slotPage3NameChanged();
	}
	else if (page==page4)
	{
		if (format!=NULL)
                {
			QString t = "<qt><dl>";
			t += i18n("<dt>Format:<dd><b>%1</b>", format->name);
			t += i18n("<dt>Source file:<dd><b>%1</b>", page2source->url().toDisplayString());
			t += i18n("<dt>New episode name:<dd><b>%1</b>", page3name->text());
                        t += "</dl>";
                        page4disp->setText(t);
	// TODO: port to KF5
// 			setButtonText(KDialog::User2,i18n("Import"));
                }
        }
        else if (page==page5)
        {
		setCursor(QCursor(Qt::WaitCursor));

		ImporterBase *importer = ImportManager::self()->createImporter(format->key);
		if (importer==NULL)
		{
			reportError(ki18n("Cannot create importer for '%1'"),format->key,false);
// 			enableButton(KDialog::User1,false);		// "Finish"
			page5disp->setText(i18n("Error while importing"));
		}
		else
		{
			QString status;
                        episodeName = page3name->text();
                        if (!importer->import(page2source->url().path(), episodeName,&status))
                        {
				if (status.isEmpty()) status = i18n("Import failed");
//                                 enableButton(KDialog::User1,false);	// "Finish"
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
//                 enableButton(KDialog::User3,false);
//                 enableButton(KDialog::Cancel,false);
        }
}


void ImportWizard::next()
{
	qDebug() << "current page" << currentPage()->name();
	bool ok = true;

	if (currentPage()==page2)
	{
		QUrl src = page2source->url();
		ok = src.isValid();
		if (ok)
		{
                        if (src.isLocalFile())
                        {
                                QFileInfo fi(src.path());
                                if (!fi.exists())
                                {
                                        reportError(ki18n("The file <b>%1</b><br>does not exist."),fi.absoluteFilePath(),false);
                                        ok = false;
                                }

                                if (ok && fi.isRelative())
                                {
                                        fi.makeAbsolute();
                                        src.setPath(fi.filePath());
                                        page2source->setUrl(src);
                                }

                                if (ok)
                                {
                                        KConfigGroup grp = KSharedConfig::openConfig()->group("Importer");
					// TODO: port to KF5
//                                         grp.writeEntry("LastLocation", page2source->fileDialog()->baseUrl());
                                }
                        }
                        else
                        {
				reportError(ki18n("Can only import from local files"), "", false);
				ok = false;
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
					reportError(ki18n("A global episode named <b>%1</b> already exists."),name,false);
                                        ok = false;
                                }
                                else if (!page3over->isChecked())
                                {
					reportError(ki18n("A user episode named <b>%1</b> already exists.<br>Select the \"Overwrite\" check box if required."),name,false);
                                        ok = false;
                                }
			}
                }
	}

	if (ok) KAssistantDialog::next();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 1:  list of formats						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage1()
{
	QWidget *w = new QWidget(this);

	QHBoxLayout *l = new QHBoxLayout(w);

	page1list = new QListWidget(w);
	page1list->setSelectionMode(QAbstractItemView::SingleSelection);
	page1list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	page1list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	page1list->setMinimumHeight(80);
	connect(page1list,SIGNAL(itemSelectionChanged()),this,SLOT(slotPage1FormatSelected()));
	l->addWidget(page1list);

        const ImportManager::FormatList *fmts = manager->allInfo();

        int maxlen = 0;
	for (ImportManager::FormatList::const_iterator it = fmts->constBegin();
		it!=fmts->constEnd(); ++it)
	{
		const ImportManager::formatInfo *fmt = (*it);
		QString s = fmt->name;

                QFontMetrics fm = page1list->fontMetrics();
                int thislen = fm.horizontalAdvance(s);
                if (thislen>maxlen) maxlen = thislen;

		page1list->addItem(s);
	}

	page1list->setFixedWidth(maxlen+page1list->verticalScrollBar()->width()+20);

	page1info = new QLabel(page1text.toString()+"<p>"+i18n("Select a format for more information about it."),w);
        page1info->setWordWrap(true);
        page1info->setOpenExternalLinks(true);
	l->addWidget(page1info,1, Qt::AlignTop);
        l->setStretchFactor(page1info,1);

	w->setMinimumHeight(240);
	page1 = addPage(w, page1caption.toString());
}


void ImportWizard::slotPage1FormatSelected()
{
	QString t = page1text.toString();

	QListWidgetItem *item = page1list->currentItem();
	QString fmtName = (item==NULL ? QString() : item->text());

	format = manager->findNamed(fmtName);
	if (format!=NULL)
	{
		t += "<p><hr>";
		if (!format->url.isEmpty())
		{
			t += "<p>"+i18n("See <a href=\"%1\">%2</a> for more information on this format.", format->url, format->url);
		}

		if (!format->notes.isEmpty())
		{
			t += "<p>"+format->notes;
		}
		else
		{
			if (format->url.isEmpty())
			{
				t += "<p>"+i18n("No further information is available on this format.");
			}
		}
		page1info->setText(t);
	}

        setValid(page1,format!=NULL);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 2:  source location						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage2()
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);

	page2info = new QLabel(page2text.subs("").toString(), w);
        page2info->setWordWrap(true);
	l->addWidget(page2info,1,Qt::AlignTop);

	page2source = new KUrlRequester(w);
	page2source->setMode(KFile::File|KFile::ExistingOnly|KFile::LocalOnly);

	page2source->fileDialog()->setWindowTitle(page2caption.toString());

        KConfigGroup grp = KSharedConfig::openConfig()->group("Importer");
	QUrl lastloc = grp.readEntry("LastLocation", QUrl());
	if (lastloc.isValid()) page2source->setStartDir(lastloc);

	connect(page2source,SIGNAL(textChanged(const QString &)),this,SLOT(slotPage2SourceSelected()));

	QLabel *lab = new QLabel(i18n("Source file:"),w);
	lab->setBuddy(page2source);
	l->addWidget(lab,1,Qt::AlignBottom);
	l->addWidget(page2source,0,Qt::AlignTop);

	page2 = addPage(w, page2caption.toString());
}


void ImportWizard::slotPage2SourceSelected()
{
	qDebug() << " url=" << page2source->url();
        setValid(page2,!page2source->url().isEmpty());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 3:  episode name						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage3()
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);

	QLabel *info = new QLabel(page3text.toString(), w);
        info->setWordWrap(true);
        info->setOpenExternalLinks(true);
	l->addWidget(info,1,Qt::AlignTop);

	page3name = new QLineEdit(w);
	connect(page3name,SIGNAL(textChanged(const QString &)),this,SLOT(slotPage3NameChanged()));

	QLabel *lab = new QLabel(i18n("Episode name:"),w);
	lab->setBuddy(page3name);
	l->addWidget(lab,1,Qt::AlignBottom);
	l->addWidget(page3name,0,Qt::AlignTop);

        page3over = new QCheckBox(i18n("Overwrite any existing episode"),w);
        l->addWidget(page3over,0,Qt::AlignLeft);

	page3 = addPage(w, page3caption.toString());
}


void ImportWizard::slotPage3NameChanged()
{
	const QString text = page3name->text();
        setValid(page3,!text.isEmpty());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 4:  ready to import						//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage4()
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);

	QLabel *info = new QLabel(page4text.toString(), w);
        info->setWordWrap(true);
	l->addWidget(info,1,Qt::AlignTop);

        page4disp = new QTextEdit(w);
        page4disp->setReadOnly(true);
        page4disp->setText("?");
        l->addWidget(page4disp,1);

	page4 = addPage(w, page4caption.toString());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Page 5:  finished							//
//									//
//////////////////////////////////////////////////////////////////////////

void ImportWizard::setupPage5()
{
	QWidget *w = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(w);

	QLabel *info = new QLabel(page5text.toString(), w);
        info->setWordWrap(true);
	l->addWidget(info,1,Qt::AlignTop);

        page5load = new QCheckBox(i18n("Load the new episode"),w);
        l->addWidget(page5load,0,Qt::AlignLeft);
        l->addSpacing(DialogBase::verticalSpacing());

        page5disp = new QTextEdit(w);
        page5disp->setReadOnly(true);
        page5disp->setText("?");
        l->addWidget(page5disp,1);

	page5 = addPage(w, page5caption.toString());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Returned results							//
//									//
////////////////////////////////////////////////////////////////////////////

QString ImportWizard::newEpisodeToLoad() const
{
	return ((importedOk && page5load->isChecked()) ? episodeName : QString());
}
