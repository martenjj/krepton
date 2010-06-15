#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		15-Jun-10					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

#########################################################################

message(STATUS "Finding episodes in ${CMAKE_CURRENT_SOURCE_DIR}...")
file(GLOB infofiles */info)
foreach (infofile ${infofiles})

  string(REGEX REPLACE "//*info$" "" subdir ${infofile})
  string(REGEX REPLACE "^.*/" "" subdir ${subdir})
  message(STATUS "  Episode '${subdir}'")

  file(GLOB files ${subdir}/*)
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" " " files ${files})
  separate_arguments(files)
  install(FILES ${files} DESTINATION ${DATA_INSTALL_DIR}/krepton/episodes/${subdir})

endforeach (infofile)
