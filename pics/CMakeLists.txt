#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		13-Jun-10					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

#########################################################################

set(krepton_PICS
  back.png
  blank.png
  crown.png
  finished.png
  key.png
  lives0.png
  lives1.png
  lives2.png
  lives3.png
  logo.png
  password.png
  pause.png
  playing.png
  splash.png
  started.png
  unknown.png
  unplayed.png
)

install(FILES ${krepton_PICS} DESTINATION ${DATA_INSTALL_DIR}/krepton/pics)
