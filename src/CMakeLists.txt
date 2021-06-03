#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		03-Jun-21					#
#########################################################################

project(krepton)

#########################################################################
#									#
#  CMake modules required here						#
#									#
#########################################################################

include(CheckLibraryExists)
include(CheckFunctionExists)
include(CheckIncludeFiles)

#########################################################################
#									#
#  Tests and options							#
#									#
#########################################################################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS I18n Config XmlGui Crash CoreAddons KIO IconThemes Auth)

find_package(Phonon4Qt5 NO_POLICY_SCOPE)
set_package_properties("Phonon4Qt5" PROPERTIES DESCRIPTION "Multimedia playback library" URL "http://phonon.kde.org" TYPE OPTIONAL PURPOSE "Required for playback of audio/video notes")
find_package(LibKFDialog REQUIRED NO_POLICY_SCOPE)
set_package_properties("LibKFDialog" PROPERTIES DESCRIPTION "KDE Frameworks dialogue utility library" URL "https://github.com/martenjj/libkfdialog" TYPE REQUIRED PURPOSE "Required to build the project")

find_package(KF5KDEGames ${KDEGAMES_MIN_VERSION})
if (KF5KDEGames_FOUND)
  set(HAVE_LIBKDEGAMES 1)
  add_definitions(-DHAVE_LIBKDEGAMES)
else (NOT KF5KDEGames_FOUND)
  message(NOTICE "libkdegames not found, high scores will not be kept")
endif (NOT KF5KDEGames_FOUND)

check_function_exists(strerror HAVE_STRERROR)
if (HAVE_STRERROR)
  add_definitions(-DHAVE_STRERROR)
  check_include_files(errno.h HAVE_ERRNO_H)
  if (HAVE_ERRNO_H)
    add_definitions(-DHAVE_ERRNO_H)
  endif (HAVE_ERRNO_H)
endif (HAVE_STRERROR)

if (SOUND_TYPE STREQUAL "libkdegames")
  if (NOT KF5KDEGames_FOUND)
    message(SEND_ERROR "libkdegames sound output requested, but package not found")
  endif (NOT KF5KDEGames_FOUND)
endif (SOUND_TYPE STREQUAL "libkdegames")

if (SOUND_TYPE STREQUAL "phonon")
  if (NOT PHONON_FOUND)
    message(SEND_ERROR "Phonon sound output requested, but package not found")
  endif (NOT PHONON_FOUND)
endif (SOUND_TYPE STREQUAL "phonon")

if (SOUND_TYPE STREQUAL "OFF")
  if (KF5KDEGames_FOUND)
    set(SOUND_TYPE "libkdegames")
  endif (SOUND_TYPE STREQUAL "OFF")
endif (KF5KDEGames_FOUND)

if (SOUND_TYPE STREQUAL "OFF")
  if (Phonon4Qt5_FOUND)
    set(SOUND_TYPE "phonon")
  endif (Phonon4Qt5_FOUND)
endif (SOUND_TYPE STREQUAL "OFF")

if (SOUND_TYPE STREQUAL "libkdegames")
  add_definitions(-DSND_KGSOUND)
  message(STATUS "Selected sound output: using KgSound")
elseif (SOUND_TYPE STREQUAL "phonon")
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

#########################################################################
#									#
#  Rigourousness							#
#									#
#########################################################################

add_definitions("-DQT_USE_FAST_CONCATENATION")
add_definitions("-DQT_USE_FAST_OPERATOR_PLUS")
add_definitions("-DQT_NO_CAST_FROM_BYTEARRAY")
add_definitions("-DQT_NO_URL_CAST_FROM_STRING")
add_definitions("-DQT_NO_CAST_TO_ASCII")
add_definitions("-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT")
add_definitions("-DQT_DEPRECATED_WARNINGS")
add_definitions("-DQT_STRICT_ITERATORS=1")

remove_definitions("-DQT_NO_CAST_FROM_ASCII")
remove_definitions("-DQT_NO_SIGNALS_SLOTS_KEYWORDS")

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
  mapplay.cpp
  mapedit.cpp
  selectgamedialog.cpp
  selectleveldialog.cpp
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
  resourcelist.cpp
)

add_executable(krepton ${krepton_SRCS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)
target_link_libraries(krepton
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  KF5::CoreAddons
  KF5::Crash
  KF5::I18n
  KF5::XmlGui
  KF5::KIOCore KF5::KIOWidgets
  KF5::IconThemes
  ${LIBKFDIALOG_LIBRARIES}
)

if (HAVE_LIBKDEGAMES)
  target_link_libraries(krepton KF5KDEGames)
endif (HAVE_LIBKDEGAMES)

if (SOUND_TYPE STREQUAL "phonon")
  add_definitions("-DHAVE_PHONON=1")
  include_directories(${PHONON_INCLUDES})
  target_link_libraries(krepton Phonon::phonon4qt5)
endif (SOUND_TYPE STREQUAL "phonon")

add_dependencies(krepton vcsversion)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

if (INSTALL_BINARIES)
  install(TARGETS krepton ${INSTALL_TARGETS_DEFAULT_ARGS})
endif (INSTALL_BINARIES)
