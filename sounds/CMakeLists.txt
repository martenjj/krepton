#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		13-Jun-10					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

#########################################################################

set(krepton_SNDS
  cage.wav
  crown.wav
  diamond.wav
  die.wav
  egg.wav
  key.wav
  monster.wav
  time.wav
  transport.wav
)

install(FILES ${krepton_SNDS} DESTINATION ${DATA_INSTALL_DIR}/krepton/sounds)
