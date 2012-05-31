#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		31-May-12					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

#########################################################################

set(EXT "ogg")
message(STATUS "Sound files are of type '${EXT}'")

set(krepton_SNDS
  cage.${EXT}
  crown.${EXT}
  diamond.${EXT}
  die.${EXT}
  egg.${EXT}
  key.${EXT}
  monster.${EXT}
  time.${EXT}
  transport.${EXT}
)

install(FILES ${krepton_SNDS} DESTINATION ${DATA_INSTALL_DIR}/krepton/sounds)

set(SOUND_FILE_EXT "\\\".${EXT}\\\"" PARENT_SCOPE)
