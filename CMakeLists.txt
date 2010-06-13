#########################################################################
#  Project:		KRepton for KDE4				#
#  Edit:		12-Jun-10					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

set(VERSION 4.3)
message(STATUS "Configuring for KRepton version ${VERSION}")

#########################################################################
#									#
#  KDE paths and definitions						#
#									#
#########################################################################

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

#########################################################################
#									#
#  Options								#
#									#
#########################################################################

option(SOUND_TYPE "specify how sound is to be played (kaudioplayer,kdeplayobject,artsplay,soxplay,alsaplay,none)" OFF)

#########################################################################
#									#
#  Subdirectories							#
#									#
#########################################################################

macro_optional_add_subdirectory(src)
macro_optional_add_subdirectory(desktop)
macro_optional_add_subdirectory(pics)
macro_optional_add_subdirectory(episodes)
macro_optional_add_subdirectory(sounds)
macro_optional_add_subdirectory(doc)

#########################################################################
#									#
#  Information at end of CMake run					#
#									#
#########################################################################

macro_display_feature_log()
