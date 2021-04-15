#########################################################################
#  Project:		KRepton for Plasma 5				#
#  Edit:		15-Apr-21					#
#########################################################################
#									#
#									#
#########################################################################

project(krepton)

set(VERSION 5.0)
message(STATUS "Configuring for KRepton version ${VERSION}")

#########################################################################
#									#
#  ECM paths and definitions						#
#									#
#########################################################################

set(QT_MIN_VERSION "5.12.0")
set(KF5_MIN_VERSION "5.60.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${KDE_INSTALL_FULL_DATAROOTDIR}/marble/cmake)

#########################################################################
#									#
#  Options								#
#									#
#########################################################################

option(SOUND_TYPE "Specify how sound is to be played (phonon,artsplay,soxplay,alsaplay,none)" OFF)
option(INSTALL_BINARIES "Install the binaries and libraries, turn off for development in place" ON)

#########################################################################
#									#
#  VCS version tracking							#
#									#
#########################################################################

add_custom_target(vcsversion
  ALL
  COMMENT "Checking VCS type and version"
  VERBATIM
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/genversion.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${VERSION}
)

#########################################################################
#									#
#  Distribution archive							#
#									#
#########################################################################

add_custom_target(dist
  VERBATIM
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/gendist.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_PROJECT_NAME} ${VERSION}
)

#########################################################################
#									#
#  Subdirectories							#
#									#
#########################################################################

add_subdirectory(sounds)
if (DEFINED SOUND_FILE_EXT)
  add_definitions(-DSOUND_FILE_EXT=${SOUND_FILE_EXT})
endif (DEFINED SOUND_FILE_EXT)

add_subdirectory(src)
add_subdirectory(desktop)
add_subdirectory(pics)
add_subdirectory(episodes)
add_subdirectory(doc)

#########################################################################
#									#
#  Information at end of CMake run					#
#									#
#########################################################################

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
