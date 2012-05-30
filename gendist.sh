#!/bin/sh
##########################################################################
##									##
##  This script is part of KRepton.					##
##									##
##  This file may be distributed and/or modified under the terms of	##
##  the GNU General Public License version 2 or any later version,	##
##  as published by the	Free Software Foundation and appearing in	##
##  the file COPYING included in the packaging of this file.		##
##									##
##  Author:  Jonathan Marten <jjm AT keelhaul DOT me DOT uk>		##
##									##
##########################################################################

set -e

SRC=$1							# CMAKE_CURRENT_SOURCE_DIR
BIN=$2							# CMAKE_CURRENT_BINARY_DIR
PRJ=$3							# CMAKE_PROJECT_NAME
VER=$4							# application version

cd $BIN

ARCH="$3-$4"						# destination directory
rm -rf $ARCH ${ARCH}.tar ${ARCH}.tar.gz			# initial clean up
mkdir $ARCH						# create as empty

echo
cp -rp $1/* $ARCH					# copy in source files
tar cvvf $ARCH.tar $ARCH				# create tar archive
gzip -9v $ARCH.tar					# compress the archive
rm -rf $ARCH						# don't need the directory

echo
echo "Created distribution archive '`pwd`/$ARCH.tar.gz'"
echo

exit 0							# successfully created
