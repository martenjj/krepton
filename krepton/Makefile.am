bin_PROGRAMS = krepton

krepton_SOURCES = main.cpp mainwindow.cpp pixmaps.cpp selectgamedialog.cpp	\
                  selectgamewidgetui.ui episodes.cpp gameplayer.cpp map.cpp	\
                  map1.cpp map2.cpp sprites.cpp scoredialog.cpp			\
                  newscoredialog.cpp gameeditor.cpp dataeditor.cpp		\
                  checkmap.cpp spriteeditor.cpp newmapdialog.cpp		\
                  newmapwidgetui.ui newtransporterdialog.cpp			\
                  newtransporterwidgetui.ui spritepreview.cpp spritegrid.cpp	\
                  mapeditor.cpp objectlist.cpp mapgrid.cpp coordlabel.cpp	\
                  objectlistbox.cpp saveepisodewidgetui.ui			\
                  saveepisodedialog.cpp sounds.cpp parentactionmapper.cpp

krepton_LDADD   =  $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)
krepton_LDFLAGS = $(all_libraries) $(KDE_RPATH)


EXTRA_DIST = krepton.desktop lo32-app-krepton.png lo16-app-krepton.png kreptonui.rc hi16-app-krepton.png hi32-app-krepton.png lo32-app-kreptoned.png lo16-app-kreptoned.png hi16-app-kreptoned.png hi32-app-kreptoned.png

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO
KDE_ICON = AUTO

xdg_apps_DATA = krepton.desktop

rcdir = $(kde_datadir)/krepton
rc_DATA = kreptonui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/krepton.pot; \
	fi
