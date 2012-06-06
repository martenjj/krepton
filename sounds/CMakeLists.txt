#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		06-Jun-12					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

set(SOUND_FILE_EXT "ogg")
message(STATUS "Sound files are of type '${SOUND_FILE_EXT}'")
set(SOUND_FILE_EXT "\\\".${SOUND_FILE_EXT}\\\"" PARENT_SCOPE)

add_subdirectory(default)
