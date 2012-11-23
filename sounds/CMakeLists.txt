#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		23-Nov-12					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

set(SOUND_FILE_EXT "ogg")
message(STATUS "Sound files are of type '${SOUND_FILE_EXT}'")
set(SOUND_FILE_EXT "\\\".${SOUND_FILE_EXT}\\\"" PARENT_SCOPE)

#########################################################################

message(STATUS "Finding sounds in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(GLOB infofiles */info)
foreach (infofile ${infofiles})

  string(REGEX REPLACE "//*info$" "" subdir ${infofile})
  string(REGEX REPLACE "^.*/" "" subdir ${subdir})
  message(STATUS "  Scheme '${subdir}'")

  file(GLOB files ${subdir}/*)
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" " " files ${files})
  separate_arguments(files)
  install(FILES ${files} DESTINATION ${DATA_INSTALL_DIR}/krepton/sounds/${subdir})

endforeach (infofile)
