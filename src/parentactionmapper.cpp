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

#include <kxmlguiclient.h>
#include <kaction.h>
#include <kdebug.h>

#include <qsignalmapper.h>

#include "parentactionmapper.h"


ParentActionMapper::ParentActionMapper(KXMLGUIClient *p,KActionCollection *ac)
{
	parent = p;
	if (parent==NULL) return;

	kdDebug(0) << k_funcinfo << "parent=" << ((void*)parent)
		   << " pac=" << ((void*)parent->actionCollection()) << endl;

	QSignalMapper *mapper = new QSignalMapper(this);
	connect(mapper,SIGNAL(mapped(const QString &)),
		this,SLOT(slotAction(const QString &)));

	KActionPtrList l = parent->actionCollection()->actions();
	for (KActionPtrList::const_iterator it = l.begin(); it!=l.end(); ++it)
	{
		const KAction *act = (const KAction *) *it;
		const KShortcut sc = act->shortcut();
		if (sc.isNull()) continue;

		kdDebug(0) << "action='" << act->name() << "'" << endl;

		KAction *newact = new KAction(act->text(),sc,mapper,SLOT(map()),
					      ac,act->name());
		mapper->setMapping(newact,act->name());
	}
}


void ParentActionMapper::slotAction(const QString &name)
{
	kdDebug(0) << k_funcinfo << "name='" << name << "'" << endl;
	if (parent==NULL) return;
	KAction *act = parent->actionCollection()->action(name);
	if (act!=NULL) act->activate();
}
