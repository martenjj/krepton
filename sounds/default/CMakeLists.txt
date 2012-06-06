#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		06-Jun-12					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

set(default_SNDS
  info
  cage.${SOUND_FILE_EXT}
  crown.${SOUND_FILE_EXT}
  diamond.${SOUND_FILE_EXT}
  die.${SOUND_FILE_EXT}
  egg.${SOUND_FILE_EXT}
  key.${SOUND_FILE_EXT}
  monster.${SOUND_FILE_EXT}
  time.${SOUND_FILE_EXT}
  transport.${SOUND_FILE_EXT}
)

install(FILES ${default_SNDS} DESTINATION ${DATA_INSTALL_DIR}/krepton/sounds/default)
