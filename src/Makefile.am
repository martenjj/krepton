bin_PROGRAMS = krepton

krepton_SOURCES = main.cpp mainwindow.cpp pixmaps.cpp selectgamedialog.cpp	\
                  episodes.cpp gameplayer.cpp map.cpp				\
                  map1.cpp map2.cpp sprites.cpp scoredialog.cpp			\
                  newscoredialog.cpp gameeditor.cpp dataeditor.cpp		\
                  checkmap.cpp spriteeditor.cpp newmapdialog.cpp		\
                  newmapwidgetui.ui newtransporterdialog.cpp			\
                  newtransporterwidgetui.ui spritepreview.cpp spritegrid.cpp	\
                  mapeditor.cpp objectlist.cpp mapgrid.cpp coordlabel.cpp	\
                  objectlistbox.cpp saveepisodewidgetui.ui			\
                  selectleveldialog.cpp 					\
                  saveepisodedialog.cpp sounds.cpp parentactionmapper.cpp	\
                  importwizard.cpp importmanager.cpp importerbase.cpp		\
                  importerbbc.cpp

krepton_LDADD   = $(LIB_ARTSKDE) $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)
krepton_LDFLAGS = $(all_libraries) $(KDE_RPATH)

EXTRA_DIST = krepton.desktop lo32-app-krepton.png lo16-app-krepton.png kreptonui.rc hi16-app-krepton.png hi32-app-krepton.png lo32-app-kreptoned.png lo16-app-kreptoned.png hi16-app-kreptoned.png hi32-app-kreptoned.png

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO
KDE_ICON = AUTO

#  Avoid nasty interaction between SCCS and the UI files.  What happens is that
#  am_edit makes all of the generated UI header files (e.g. selectgamewidgetui.h
#  which is made from selectgamewidgetui.ui by UIC) dependencies of all the
#  source files, e.g.
#
#    $(srcdir)/main.cpp: selectgamewidgetui.h newmapwidgetui.h ...
#
#  This is to ensure that the 'ui.h' files are generated before any source
#  code is compiled, because these 'ui.h' files are in turn included by the
#  standard 'h' headers - yet they are not dependency tracked.
#
#  So far so good - starting from a clean source directory, the 'ui.h' headers
#  are generated first.  But then make(1) decides that main.cpp is out of date
#  with respect to the 'ui.h' headers and makes it unconditionally using its
#  standard rule:
#
#    sccs get -Gmain.cpp SCCS/s.main.cpp
#
#  which is benign if main.cpp does not exist (i.e. has been sccs clean'ed) or
#  if it is checked out read-only.  But if it is checked out for editing, this
#  will fail with "File exists and is writable".
#
#  The redefinition of $(GET) here avoids that without having to modify am_edit
#  or any of the other standard KDE admin stuff.
GET = test -e SCCS/p.$@ || /usr/sccs/get

xdg_apps_DATA = krepton.desktop

rcdir = $(kde_datadir)/krepton
rc_DATA = kreptonui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/krepton.pot; \
	fi
