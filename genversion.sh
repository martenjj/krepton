#!/bin/sh
##########################################################################
##									##
##  Project:	SCM Tools - Version Control				##
##									##
##  This is the master script for embedding a build version into an	##
##  application binary.  It works with SVN and GIT.			##
##									##
##  See the file README for details of how to use this script.		##
##									##
##  This file may be distributed and/or modified under the terms of	##
##  the GNU General Public License version 2, as published by the	##
##  Free Software Foundation and appearing in the file COPYING		##
##  included in the packaging of this file.				##
##									##
##  Author:  Jonathan Marten <jjm AT keelhaul DOT me DOT uk>		##
##									##
##########################################################################

SCRIPT=`basename $0 .sh`

SRC=$1							# CMAKE_CURRENT_SOURCE_DIR
BIN=$2							# CMAKE_CURRENT_BINARY_DIR
VER=$3							# application version

SVNCMD=`PATH=$PATH:$KDEDIR/bin type -p svn 2>/dev/null`
GITCMD=`PATH=$PATH:$KDEDIR/bin type -p git 2>/dev/null`

VCS=							# version control found
REV=							# version from that

if [ -d "$SRC/.svn" ]					# source is under SVN
then
	VCS="SVN"
	if [ -n "$SVNCMD" ]				# SVN command available
	then
		REV=`$SVNCMD info $SRC 2>/dev/null | sed -n -e'/^Revision:/ { s/^[^:]*: *//;p;q }'`
	else						# SVN command not found
		REV="Unknown"
	fi
elif [ -d "$SRC/.git" ]					# source is under GIT
then
	VCS="GIT"
	if [ -n "$GITCMD" ]				# GIT command available
	then						# first try formatted version
		REV=`cd $1 && $GITCMD describe 2>/dev/null`
		if [ ! -n "$REV" ]			# if not available then
		then					# hash of last commit
			REV=`cd $1 && $GITCMD log -1 --abbrev-commit | sed -e 's/commit  *//;q'`
		fi

		# Enforce Git tagging of the source to match the version
		TAG=`cd $1 && git describe --tags --abbrev=0`
		if [ \( -n "$TAG" \) -a \( "$TAG" != "$VER" \) ]
		then
			echo "${SCRIPT} (${C_YELLOW}${C_BOLD}WARNING${C_NORM}): ${C_YELLOW}${C_BOLD}Latest GIT tag '$TAG' does not match version '$VER'${C_NORM}" >&2
		fi
	else						# GIT command not found
		REV="Unknown"
	fi
fi

TMPFILE="$BIN/vcsversion.h.tmp"				# temporary header file
{
	echo "#ifndef VERSION"
	echo "#define VERSION              \"${VER}\""
	echo "#endif"
	echo
	echo "#define VCS_TYPE             \"${VCS}\""
	echo "#define VCS_REVISION         \"${REV}\""
	if [ -n "$VCS" ]
	then
		echo "#define VCS_AVAILABLE        1"
	else
		echo "#define VCS_AVAILABLE        0"
	fi
	echo
} >$TMPFILE

OUTFILE="$BIN/vcsversion.h"				# the real header file
if [ ! -f $OUTFILE ]					# does not exist yet
then
	echo "${SCRIPT} (${C_GREEN}INFO${C_NORM}): ${C_GREEN}Creating '$OUTFILE'...${C_NORM}" >&2
else							# already exists
	if cmp -s $TMPFILE $OUTFILE			# is it still current?
	then
		rm $TMPFILE				# yes, nothing to do
		exit 0
	else
		echo "${SCRIPT} (${C_GREEN}INFO${C_NORM}): ${C_GREEN}Updating '$OUTFILE'...${C_NORM}" >&2
	fi
fi

mv $TMPFILE $OUTFILE					# update the header file
if [ -n "$VCS" ]
then
	echo "${SCRIPT} (${C_GREEN}INFO${C_NORM}): ${C_GREEN}Current $VCS revision is '$REV'${C_NORM}" >&2
fi
exit 0
