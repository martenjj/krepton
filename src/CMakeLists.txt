#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		03-Jun-12					#
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

if (SOUND_TYPE STREQUAL "phonon")
  if (NOT PHONON_FOUND)
    message(SEND_ERROR "Phonon sound output requested, but not found")
  endif (NOT PHONON_FOUND)
endif (SOUND_TYPE STREQUAL "phonon")

if (SOUND_TYPE STREQUAL "OFF")
  if (PHONON_FOUND)
    set(SOUND_TYPE "phonon")
  endif (PHONON_FOUND)
endif (SOUND_TYPE STREQUAL "OFF")

if (SOUND_TYPE STREQUAL "phonon")
  add_definitions(-DSND_PHONON)
  message(STATUS "Selected sound output: using Phonon")
elseif (SOUND_TYPE STREQUAL "artsplay")
  add_definitions(-DSND_EXTERNAL -DEXT_ARTSPLAY)
  message(STATUS "Selected sound output: external artsplay")
elseif (SOUND_TYPE STREQUAL "soxplay")
  add_definitions(-DSND_EXTERNAL -DEXT_SOXPLAY)
  message(STATUS "Selected sound output: external soxplay")
elseif (SOUND_TYPE STREQUAL "alsaplay")
  add_definitions(-DSND_EXTERNAL -DEXT_ALSAPLAY)
  message(STATUS "Selected sound output: external alsaplay ")
elseif (SOUND_TYPE STREQUAL "none")
  message(STATUS "Selected sound output: none")
else (SOUND_TYPE STREQUAL "phonon")
  message(SEND_ERROR "Unrecognised SOUND_TYPE option '${SOUND_TYPE}'")
endif (SOUND_TYPE STREQUAL "phonon")

add_definitions(-DVERSION=\\"${VERSION}\\")
add_definitions(-DQT_STRICT_ITERATORS=1)

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
  objectlistbox.cpp
  saveepisodedialog.cpp
  importwizard.cpp
  importmanager.cpp
  importerbase.cpp
  importerbbc.cpp
  cheatdialog.cpp
)

kde4_add_executable(krepton ${krepton_SRCS})
target_link_libraries(krepton ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS})
if (SOUND_TYPE STREQUAL "phonon")
  target_link_libraries(krepton ${PHONON_LIBS})
endif (SOUND_TYPE STREQUAL "phonon")

add_dependencies(krepton vcsversion)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

install(TARGETS krepton ${INSTALL_TARGETS_DEFAULT_ARGS})
