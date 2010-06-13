#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		13-Jun-10					#
#########################################################################

project(krepton)

#########################################################################
#									#
#  Tests and options							#
#									#
#########################################################################

check_library_exists(m fabs "" HAVE_LIBM)
if (HAVE_LIBM)
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} "-lm")
endif (HAVE_LIBM)

check_function_exists(strerror HAVE_STRERROR)
if (HAVE_STRERROR)
  add_definitions(-DHAVE_STRERROR)
  check_include_files(errno.h HAVE_ERRNO_H)
  if (HAVE_ERRNO_H)
  add_definitions(-DHAVE_ERRNO_H)
  endif (HAVE_ERRNO_H)
endif (HAVE_STRERROR)

check_function_exists(pow HAVE_FUNCTION_POW)
if (NOT HAVE_FUNCTION_POW)
  message(SEND_ERROR "Function 'pow' is not available")
endif (NOT HAVE_FUNCTION_POW)

check_function_exists(sqrt HAVE_FUNCTION_SQRT)
if (NOT HAVE_FUNCTION_SQRT)
  message(SEND_ERROR "Function 'sqrt' is not available")
endif (NOT HAVE_FUNCTION_SQRT)

#check_include_file(sys/stat.h HAVE_SYS_STAT_H)
#check_include_file(sys/types.h HAVE_SYS_TYPES_H)

##if ((SOUND_TYPE STREQUAL "OFF") OR (SOUND_TYPE STREQUAL "kaudioplayer"))
if (SOUND_TYPE STREQUAL "kaudioplayer")
  add_definitions(-DSND_AUDIOPLAYER)
  message(STATUS "Selected sound output: using KAudioPlayer")
elseif (SOUND_TYPE STREQUAL "kdeplayobject")
  add_definitions(-DSND_PLAYOBJECT)
  message(STATUS "Selected sound output: using KDE::PlayObject")
elseif (SOUND_TYPE STREQUAL "artsplay")
  add_definitions(-DSND_EXTERNAL -DEXT_ARTSPLAY)
  message(STATUS "Selected sound output: external artsplay")
elseif (SOUND_TYPE STREQUAL "soxplay")
  add_definitions(-DSND_EXTERNAL -DEXT_SOXPLAY)
  message(STATUS "Selected sound output: external soxplay")
elseif (SOUND_TYPE STREQUAL "alsaplay")
  add_definitions(-DSND_EXTERNAL -DEXT_ALSAPLAY)
  message(STATUS "Selected sound output: external alsaplay ")
##elseif (SOUND_TYPE STREQUAL "none")
elseif ((SOUND_TYPE STREQUAL "none") OR (SOUND_TYPE STREQUAL "OFF"))
  message(STATUS "Selected sound output: none")
##else ((SOUND_TYPE STREQUAL "OFF") OR (SOUND_TYPE STREQUAL "kaudioplayer"))
else (SOUND_TYPE STREQUAL "kaudioplayer")
  message(SEND_ERROR "Unrecognised SOUND_TYPE option '${SOUND_TYPE}'")
##endif ((SOUND_TYPE STREQUAL "OFF") OR (SOUND_TYPE STREQUAL "kaudioplayer"))
endif (SOUND_TYPE STREQUAL "kaudioplayer")

add_definitions(-DVERSION=\\"${VERSION}\\")
add_definitions(-DQT3_SUPPORT=1)

#########################################################################
#									#
#  Building the target							#
#									#
#########################################################################

set(krepton_SRCS
  main.cpp
  mainwindow.cpp
  pixmaps.cpp
  episodes.cpp
  sprites.cpp
  sounds.cpp
  gameplayer.cpp
  map.cpp
  map1.cpp
  map2.cpp
  selectgamedialog.cpp
  selectleveldialog.cpp
  scoredialog.cpp
  newscoredialog.cpp
  gameeditor.cpp
  dataeditor.cpp
  mapeditor.cpp
  spriteeditor.cpp
  checkmap.cpp
  newmapdialog.cpp
  newtransporterdialog.cpp
  spritepreview.cpp
  spritegrid.cpp
  objectlist.cpp
  mapgrid.cpp
  coordlabel.cpp
  objectlistbox.cpp
  saveepisodedialog.cpp
  importwizard.cpp
  importmanager.cpp
  importerbase.cpp
  importerbbc.cpp
#  parentactionmapper.cpp
)

kde4_add_ui3_files(krepton_SRCS
  saveepisodewidgetui.ui
  newtransporterwidgetui.ui
  newmapwidgetui.ui
)

kde4_add_executable(krepton ${krepton_SRCS})
target_link_libraries(krepton ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS}
  ${KDE4_KDE3SUPPORT_LIBS} ${QT_QT3SUPPORT_LIBRARY})

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

install(TARGETS krepton ${INSTALL_TARGETS_DEFAULT_ARGS})
