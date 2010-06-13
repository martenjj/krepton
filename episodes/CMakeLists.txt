#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		13-Jun-10					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

#########################################################################

set(krepton_EPISODES
  blank
  simple

  prelude
  toccata
  finale

  america
  oceans
  arctic
  africa
  orient

  baby
  school
  teens
  work
  oap

  prehistory
  egypt
  victoria
  now
  future

  pirate
  preludee
  babyj
)

message(STATUS "Finding episodes in ${CMAKE_CURRENT_SOURCE_DIR}...")
foreach (subdir ${krepton_EPISODES})

  execute_process(COMMAND "sh" "-c" "ls -1 ${subdir}/* | tr '\\n' ' '"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE files)
  message(STATUS "Episode '${subdir}' files: ${files}")
  separate_arguments(files)

  install(FILES ${files} DESTINATION ${DATA_INSTALL_DIR}/krepton/episodes/${subdir})

endforeach (subdir)
