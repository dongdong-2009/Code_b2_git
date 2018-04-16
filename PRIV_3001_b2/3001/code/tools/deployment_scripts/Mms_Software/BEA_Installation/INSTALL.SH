#!/bin/sh

#       Copyright 1998 BEA Systems, Inc.
#       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF
#       BEA Systems, Inc.
#       The copyright notice above does not evidence any
#       actual or intended publication of such source code. 

#ident	"@(#) messageq/src/all/mb/install.sh	$Revision: 1.1.2.1 $"
#################################################
# GENERIC FUNCTIONS USED FOR INSTALLATION - DO NOT CHANGE
#
# ckfuncs:  Installation functions that mimic SVR4 ck* utilities
#

ckitem()
{

	unset QOPT
	unset LABEL
	unset DEFAULT
	unset HELP
	unset INVIS
	unset ERROR
	unset PROMPT
	unset PROMPTSUF
	unset REALPROMPT
	unset MENU

	DEFAULTHELP=`cat <<!
        Enter the number of the menu item you wish to select, or the token
        which is associated with the menu item.   Enter ?? to reprint
        the menu.
!`

	DEFAULTERROR=`cat <<!
ERROR:
        Entry does not match any menu selection. Enter the number of the
        menu item you wish to select, or the token which is associated with
        the menu item.  Enter ?? to reprint the menu.
!`
	USAGE="usage: $0"
	OPTIONS="QWunof:l:i:m:d:h:e:p:k:s:"
	# set -- `getopt "$OPTIONS" $*`
	if [ $? != 0 ] ; then
		echo "$USAGE" >&2
		exit 1
	fi

	while [ -n "$1" ] ; do
		case "$1" in
		-Q) QOPT=yes ; shift ;;
		-W) echo "-W not implemented" >&2 ; shift 2 ;;
		-u|-n|-o) echo "$1 not implemented" >&2 ; shift ;;
		-f) echo "-f not implemented" >&2 ; shift 2 ;;
		-l) LABEL="$2" ; shift 2 ;;
		-i) INVIS="$INVIS $2" ; shift 2 ;;
		-m) echo "$1 not implemented" >&2 ; shift 2 ;;
		-d) DEFAULT="$2" ; shift 2 ;;
		-h) HELP="        $2" ; shift 2 ;;
		-e) ERROR="ERROR: $2" ; shift 2 ;;
		-p) PROMPT="$2" ; shift 2 ;;
		-k|-s) echo "$1 not implemented" >&2 ; shift 2 ;;
	    	# --) shift ; break ;;
		*) break ;;
		esac
	done

	if [ -n "$QOPT" ] ; then
		PROMPTSUF="[?,??]"
	else
		PROMPTSUF="[?,??,q]"
	fi
	if [ -n "$DEFAULT" ] ; then
		PROMPTSUF="(default: $DEFAULT) $PROMPTSUF"
	fi
	if [ -z "$HELP" ] ; then
		HELP="$DEFAULTHELP"
	fi
	if [ -z "$PROMPT" ] ; then
		PROMPT="Enter selection"
	fi
	if [ -z "$ERROR" ] ; then
		ERROR="$DEFAULTERROR"
	fi
	REALPROMPT="\n$PROMPT $PROMPTSUF: \c"
	n=1
	while [ "$#" -gt 0 ] ; do
		if [ -z "$1" ] ; then
			shift
			continue
		fi
		tok=`echo "$1" | cut -f1`
		text=`echo "$1" | cut -f2`
		len=`expr $tok : ".*"`
		tabs="	"
		if [ "$len" -lt 8 ] ; then
			tabs="		"
		fi
		for i in $INVIS ; do
			if [ "$i" = "$tok" ] ; then
				break
			fi
		done
		if [ "$i" != "$tok" ] ; then
			MENU="$MENU
  $n	$tok$tabs$text"
		fi
		KEYS="$KEYS	$n@$tok"
		n=`expr $n + 1 2>/dev/null`
		shift
	done

	if [ -n "$LABEL" ] ; then
		echo "\n$LABEL" >&2
	fi
	echo "$MENU" >&2

	while true ; do
		good=
		echo "$REALPROMPT" >&2
		read ANSWER
		if [ -z "$ANSWER" ] ; then
			if [ -n "$DEFAULT" ] ; then
				echo "$DEFAULT"
				return 0
			else
				echo "ERROR: Input is required" >&2
				continue
			fi
		fi
		for j in $ANSWER ; do
			case "$j" in
			q)	if [ -z "$QOPT" -a "$ANSWER" = q ] ; then
					echo "q"
					return 3
				fi
				;;
			\?)	echo "$HELP" >&2
				continue 2
				;;
			\?\?)	echo "$MENU" >&2
				continue 2
				;;
			esac
			for i in $KEYS ; do
				num=`echo $i | cut -f1 -d@`
				tok=`echo $i | cut -f2 -d@`
				if [ "$j" = "$num" -o "$j" = "$tok" ] ; then
					good="$good $tok"
					continue 2
				fi
			done
			echo "$ERROR" >&2
			continue 2
		done
		# Get rid of trailing/leading spaces
		good=`echo $good`
		echo "$good"
		return 0
	done
}


ckpath()
{

	unset QOPT
	unset ABSOLUTE
	unset ISDIRECTORY
	unset ISFILE
	unset LABEL
	unset DEFAULT
	unset HELP
	unset ERROR
	unset PROMPT
	unset PROMPTSUF
	unset REALPROMPT

	DEFAULTHELP=`cat <<!
        A pathname is a filename, optionally preceded by parent directories.
!`

	DEFAULTERROR=`cat <<!
ERROR:
        Illegal pathname.
!`

	USAGE="usage: $0"
	OPTIONS="QWalrtwxd:h:e:p:k:s:bcfynoz"
	# set -- `getopt "$OPTIONS" $*`
	if [ $? != 0 ] ; then
		echo "$USAGE" >&2
		exit 1
	fi

	while [ -n "$1" ] ; do
		case "$1" in
		-Q) QOPT=yes ; shift ;;
		-W) echo "-W not implemented" >&2 ; shift 2 ;;
		-a) ABSOLUTE=yes ; shift ;;
		-l|-r|-t|-w|-x) echo "$1 not implemented" >&2 ; shift ;;
		-d) DEFAULT="$2" ; shift 2 ;;
		-h) HELP="        $2" ; shift 2 ;;
		-e) ERROR="ERROR: $2" ; shift 2 ;;
		-p) PROMPT="$2" ; shift 2 ;;
		-k|-s) echo "$1 not implemented" >&2 ; shift 2 ;;
		-b|-c|-n|-o|-z) echo "$1 not implemented" >&2 ; shift ;;
		-f) ISFILE=yes ; shift ;;
		-y) ISDIRECTORY=yes ; shift ;;
	    	# --) shift ; break ;;
		*) break ;;
		esac
	done

	if [ -n "$ISFILE" -a -n "$ISDIRECTORY" ] ; then
		echo "ERROR: mutually exclusive options used (-f, -y)" >&2
		return 4
	fi
	if [ -n "$QOPT" ] ; then
		PROMPTSUF="[?]"
	else
		PROMPTSUF="[?,q]"
	fi
	if [ -n "$DEFAULT" ] ; then
		PROMPTSUF="(default: $DEFAULT) $PROMPTSUF"
	fi
	if [ -z "$HELP" ] ; then
		HELP="$DEFAULTHELP"
		if [ -n "$ABSOLUTE" ] ; then
			HELP="$HELP
        - The pathname must begin with a slash (/)."
		fi
		if [ -n "$ISDIRECTORY" ] ; then
			HELP="$HELP
        - The pathname must specify a directory."
		fi
	fi
	if [ -z "$PROMPT" ] ; then
		if [ -n "$ABSOLUTE" ] ; then
			PROMPT="Enter an absolute pathname"
		else
			PROMPT="Enter a pathname"
		fi
	fi
	if [ -z "$ERROR" ] ; then
		ERROR="$DEFAULTERROR"
		if [ -n "$ABSOLUTE" ] ; then
			ERROR="$ERROR
        - Pathname must begin with a slash (/)."
		fi
		if [ -n "$ISDIRECTORY" ] ; then
			ERROR="$ERROR
        - Pathname must specify a directory."
		fi
	fi
	REALPROMPT="\n$PROMPT $PROMPTSUF: \c"

	while true ; do
		echo "$REALPROMPT" >&2
		read ANSWER
		case "$ANSWER" in
		q)	if [ -z "$QOPT" ] ; then
				echo "q"
				return 3
			fi
			if [ -z "$ABSOLUTE" ] ; then
				if [ -z "$ISDIRECTORY" -o -d "$ANSWER" ] ; then
					echo "$ANSWER"
					return 0
				fi
				if [ -z "$ISFILE" -o -f "$ANSWER" ] ; then
					echo "$ANSWER"
					return 0
				fi
			fi
			;;
		\?)	echo "$HELP" >&2
			continue
			;;
		/*)	if [ -z "$ISDIRECTORY" -o -d "$ANSWER" ] ; then
				echo "$ANSWER"
				return 0
			fi
			if [ -z "$ISFILE" -o -f "$ANSWER" ] ; then
				echo "$ANSWER"
				return 0
			fi
			;;
		"")	if [ -n "$DEFAULT" ] ; then
				echo "$DEFAULT"
				return 0
			fi
			;;
		*)	if [ -z "$ABSOLUTE" ] ; then
				if [ -z "$ISDIRECTORY" -o -d "$ANSWER" ] ; then
					echo "$ANSWER"
					return 0
				fi
				if [ -z "$ISFILE" -o -f "$ANSWER" ] ; then
					echo "$ANSWER"
					return 0
				fi
			fi
			;;
		esac
		echo "$ERROR" >&2
	done
}


ckyorn()
{

	unset QOPT
	unset DEFAULT
	unset HELP
	unset ERROR
	unset PROMPT
	unset PROMPTSUF
	unset REALPROMPT

	DEFAULTHELP=`cat <<!
        Enter y or yes if your answer is yes; n or no if your answer is no.
!`

	DEFAULTERROR=`cat <<!
ERROR: Please enter yes or no.
!`

	USAGE="usage: $0"
	OPTIONS="QWd:h:e:p:k:s:"
	# set -- `getopt "$OPTIONS" $*`
	if [ $? != 0 ] ; then
		echo "$USAGE" >&2
		exit 1
	fi

	while [ -n "$1" ] ; do
		case "$1" in
		-Q) QOPT=yes ; shift ;;
		-W) echo "-W not implemented" >&2 ; shift 2 ;;
		-d) DEFAULT="$2" ; shift 2 ;;
		-h) HELP="        $2" ; shift 2 ;;
		-e) ERROR="ERROR: $2" ; shift 2 ;;
		-p) PROMPT="$2" ; shift 2 ;;
		-k|-s) echo "$1 not implemented" >&2 ; shift 2 ;;
	    	# --) shift ; break ;;
		*) break ;;
		esac
	done

	if [ -n "$QOPT" ] ; then
		PROMPTSUF="[y,n,?]"
	else
		PROMPTSUF="[y,n,?,q]"
	fi
	if [ -n "$DEFAULT" ] ; then
		PROMPTSUF="(default: $DEFAULT) $PROMPTSUF"
	fi
	if [ -z "$HELP" ] ; then
		HELP="$DEFAULTHELP"
	fi
	if [ -z "$PROMPT" ] ; then
		PROMPT="Yes or No"
	fi
	if [ -z "$ERROR" ] ; then
		ERROR="$DEFAULTERROR"
	fi
	REALPROMPT="\n$PROMPT $PROMPTSUF: \c"

	while true ; do
		echo "$REALPROMPT" >&2
		read ANSWER
		case "$ANSWER" in
		q)	if [ -z "$QOPT" ] ; then
				echo "q"
				return 3
			fi
			;;
		\?)	echo "$HELP" >&2
			continue
			;;
		y|yes|Y|YES|n|no|N|NO)
			echo "$ANSWER"
			return 0
			;;
		"")	if [ -n "$DEFAULT" ] ; then
				echo "$DEFAULT"
				return 0
			fi
			;;
		esac
		echo "$ERROR" >&2
	done
}
#
# Mimics the SVR4 version of the function. Put here because it
# is part of an optional package (oam) on SVR4 an may not be installed.
# Also used on the non-SVR4 machines.
#
ckstr()
{

	unset QOPT
	unset REGEXP
	unset LABEL
	unset DEFAULT
	unset HELP
	unset ERROR
	unset PROMPT
	unset PROMPTSUF
	unset REALPROMPT
	unset LENGTH

	DEFAULTHELP=`cat <<!
        Please enter a string.
!`

	DEFAULTERROR=`cat <<!
ERROR:
        Entry does not match available menu selection. Enter the number of the
        menu item you wish to select or the token which is associated with
        the menu item.  Enter ?? to reprint the menu.
!`
	USAGE="usage: $0"
	OPTIONS="QWr:l:d:h:e:p:k:s:"
	# set -- `getopt "$OPTIONS" $*`
	if [ $? != 0 ] ; then
		echo "$USAGE" >&2
		exit 1
	fi

	while [ -n "$1" ] ; do
		case "$1" in
		-Q) QOPT=yes ; shift ;;
		-W) echo "-W not implemented" >&2 ; shift 2 ;;
		-r) REGEXP="$2" ; shift 2 ;;
		-l) LENGTH="$2" ; shift 2 ;;
		-d) DEFAULT="$2" ; shift 2 ;;
		-h) HELP="        $2" ; shift 2 ;;
		-e) ERROR="ERROR: $2" ; shift 2 ;;
		-p) PROMPT="$2" ; shift 2 ;;
		-k|-s) echo "$1 not implemented" >&2 ; shift 2 ;;
	    	# --) shift ; break ;;
		*) break ;;
		esac
	done

	if [ -n "$QOPT" ] ; then
		PROMPTSUF="[?]"
	else
		PROMPTSUF="[?,q]"
	fi
	if [ -n "$DEFAULT" ] ; then
		PROMPTSUF="(default: $DEFAULT) $PROMPTSUF"
	fi
	if [ -z "$HELP" ] ; then
		HELP="$DEFAULTHELP"
		if [ -n "$LENGTH" ] ; then
			HELP="$HELP
        - containing no more than $LENGTH characters"
		fi
		if [ -n "$REGEXP" ] ; then
			HELP="$HELP
        - matching the pattern $REGEXP"
		else
			HELP="$HELP
        - containing no embedded, leading, or trailing spaces or tabs"
		fi
	fi
	if [ -z "$PROMPT" ] ; then
		PROMPT="Enter selection"
	fi
	if [ -z "$ERROR" ] ; then
		ERROR="$DEFAULTERROR"
	fi
	REALPROMPT="\n$PROMPT $PROMPTSUF: \c"

	while true ; do
		echo "$REALPROMPT" >&2
		read ANSWER
		case "$ANSWER" in
		q)	if [ -z "$QOPT" ] ; then
				echo "q"
				return 3
			fi
			# fall through - might match
			;;
		\?)	echo "$HELP" >&2
			continue
			;;
		"")	if [ -n "$DEFAULT" ] ; then
				echo "$DEFAULT"
				return 0
			fi
			echo "ERROR: Input is required" >&2
			continue
			;;
		esac
		# can't use expr length on Solaris 2.x
		len=`expr "$ANSWER" : '.*'`
		if [ -n "$LENGTH" ] ; then
			if [ "$len" -gt "$LENGTH" ] ; then
				echo "$ERROR" >&2
				continue
			fi
		fi
		if [ -n "$REGEXP" ] ; then
			match=`expr "$ANSWER" : "$REGEXP"`
			if [ "$match" -ne "$len" ] ; then
				echo "$ERROR" >&2
				continue
			fi
		fi
		echo "$ANSWER"
		return 0
	done
}
#
#
# OTHER GENERIC INSTALLATION FUNCTIONS
#

#
# checkSpace: See if enough space is available in $1
# SPACE lists requirements of each class
# Return 0 if enough space, 1 if not enough.
#

checkSpace()
{

	echo "\nDetermining if sufficient space is available ..." >&2

	blks_needed=0
	spacefile=`echo [Ss][Pp][Aa][Cc][Ee]*`

	if [ -f "$spacefile" ] ; then
		for i in $CLASSES ; do
			blks=`grep "^$i	" < $spacefile | cut -d"	" -f2 2>/dev/null`
			if [ -n "$blks" ] ; then
				blks_needed=`expr $blks + $blks_needed 2>/dev/null`
			fi
		done
	fi
	if [ -z "$blks_needed" ] ; then
		echo "ERROR: Can't calculate space requirements" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 1
	fi

	echo "$blks_needed blocks are required" >&2
	blks_avail=`getFreeBlks $1 2>/dev/null`
	if [ -z "$blks_avail" ] ; then
		echo "ERROR: Can't determine available space in $1" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 1
	fi
	echo "$blks_avail blocks are available to $1" >&2
	if [ "$blks_needed" -gt "$blks_avail" ] ; then
		echo "ERROR: Insufficient space in $1; please choose another directory" >&2
		return 1
	fi
	return 0
}


#
# unloadFile: uncompress/cpio file specified by $1 into $2
# Some systems don't report non-0 exit code for cpio failure,
# so assume failure if "errno" appears in output.
#

unloadFile()
{

	if [ -z "$2" -o ! -x "$2" ] ; then
		echo "ERROR: Can't cd to $2" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 1
	fi
	cd $2
	if [ $? -ne 0 ] ; then
		echo "ERROR: Can't cd to $2" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 1
	fi
	tmp=/tmp/BEA2.$$
	> $tmp

	echo "\nUnloading $1 ..." >&2
	uncompress < $1 | cpio -ictB 2>/dev/null | xargs rm -f > $tmp 2>&1
	if [ -n "`cat $tmp`" ] ; then
		echo "ERROR: Can't overwrite files in $2" >&2
		echo "Please ensure that $2 is empty and retry the installation" >&2
		doExit 1
	fi
	uncompress < $1 | cpio -vicdumB 2>&1 | tee $tmp
	if [ $? -ne 0 -o -n "`grep -v tperrno $tmp | grep errno`" ] ; then
		echo "ERROR: Can't unload $1 into $2" >&2
		echo "Please ensure that $2 is empty and retry the installation" >&2
		doExit 1
	fi
	# Try to change group and user id's in case we are running
	# as root
	uid=`id | sed -e 's/uid=//' -e 's/(.*//'`
	gid=`id | sed -e 's/.*gid=//' -e 's/(.*//'`
	uncompress < $1 | cpio -ictB 2>/dev/null | xargs chgrp $gid 2>/dev/null
	uncompress < $1 | cpio -ictB 2>/dev/null | xargs chown $uid 2>/dev/null
	echo "... finished" >&2
	rm -f $tmp
}

#
# doExit: exit install script.
# $1 is exit code, assumed to be non-zero (bad).
# Assumes error message has already been output.
# Also called if 'quit' is chosen.
#

doExit()
{

	rm -f /tmp/BEA.$$ /tmp/BEA1.$$ /tmp/BEA2.$$ /tmp/BEA3.$$

	echo "\nInstallation terminated" >&2

	exit $1

}

#
# dotPKGINFO: dot in PKGINFO file in current directory
#

dotPKGINFO()
{

	pkginfofile=`echo [Pp][Kk][Gg][Ii][Nn][Ff][Oo]*`
	if [ ! -r "$pkginfofile" ] ; then
		echo "ERROR: pkginfo file missing" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 3
	fi
	. ./$pkginfofile >/dev/null 2>&1
	if [ $? -ne 0 ] ; then
		echo "ERROR: Can't dot pkginfo file" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 3
	fi
}


#
# installPkg: install package named $1, located in directory $dir
# Mimics pkgadd
# Returns 0 if ok, otherwise failed
#

installPkg()
{

	pkg=$1
	dir=$2

	cd $2 2>/dev/null
	if [ $? -ne 0 ] ; then
		echo "ERROR: Can't install unknown package <$i>" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 3
	fi

	#
	# Set up environment: dot pkginfo, etc.
	#
	CLASSES=
	DOT=
	dotPKGINFO
	echo "" >&2
	echo "$NAME" >&2
	echo "($ARCH) $VERSION" >&2
	copyrightfile=`echo [Cc][Oo][Pp][Yy][Rr][Ii][Gg][Hh]*`
	if [ -f "$copyrightfile" ] ; then
		cat $copyrightfile >&2
	fi
	#
	# Execute the pre-install function - one should exist for
	# each of the installable packages. The PREINSTALL and 
	# POSTINSTALL variables are set in the installPkgs function.
	# These functions are executed here to tailor the installation 
	# specifically to the package being installed.
	#
	eval $PREINSTALL

	#
	# SKIPINST = "yes" indicates a reinstall of same product/release with
	# successful verification of previously installed media.  We'll 
	# execute POSTINSTALL, then skip the media install via a return.
	#
	if [ "$SKIPINST" = "yes" ] ; then
		eval $POSTINSTALL
		echo "\nUpgrade/Re-installation of "$NAME" was successful" >&2
		return 0
	fi

	if [ -z "$DOT" -o -z "$CLASSES" ] ; then
		echo "ERROR: Request script failed" >&2
		echo "Please contact customer support for assistance" >&2
		doExit 3
	fi


	#
	# Since there is only one doc package on the cd, and it is
	# stored in directory structure, we do not need to use the 
	# unloadFile() routine for doc. If WEBDOCONLY is yes, then we need
	# to copy only those files that are in "webdoc.lst", otherwise
	# copy the entire directory structure.
	#
	if [ "${CLASSES}" = "tuxdoc" -o "${CLASSES}" = "msgqdoc" -o "${CLASSES}" = "mqscdoc" ]; then
		#
		# Which case does the CD driver display the filenames on
		# the CD? This call is needed for installing doc, but may
		# be useful for other packages in the future.
		#
		UpperCase

		#
		# Since the doc is no longer packaged with each
		# platform, make sure the clear text copy from
		# which we are to copy is installed on the CD. It
		# may be the case that for some loads it is not.
		#
		DocOnCD 
		if [ "$?" -eq 1 ]; then
			echo "ERROR: Documentation is not on the CD." >&2
			echo "Please contact customer support for assistance." >&2
			doExit 5
		fi

		
		#
		# Documentation is on the CD, let's install it keeping
		# track of the current directory so that when we are
		# done, we can return to it.
		#
		cwd=`pwd`
		cd $CDROOT
		echo "\nInstalling documentation..." >&2

		if [ "${uppercase}" = TRUE ]; then
			#
			# Remove the contents of any previously installed
			# documentation for this product/version before
			# trying to install. There may be old copies
			# of files that will hamper the installation.
			#
			rm -rf $DOCDIR/doc/$docsubdir 2>/dev/null
			
			#
			# We can now install into a clean directory.
			#
			find DOC -type f -print | cpio -dumpv $DOCDIR 2>/dev/null
		else
			#
			# Remove the contents of any previously installed
			# documentation for this product/version before
			# trying to install. There may be old copies
			# of files that will hamper the installation.
			#
			rm -rf $DOCDIR/doc/$docsubdir 2>/dev/null

			#
			# We can now install into a clean directory.
			#
			find doc -type f -print | cpio -dumpv $DOCDIR 2>/dev/null
		fi

		if [ $? -ne 0 ] ; then
			echo "ERROR: Problem unloading documentation..." >&2
			echo "Please contact customer support for assistance" >&2
			doExit 1
		fi

		#
		# Go back to the directory we were in.
		#
		cd $cwd

	elif [ "${CLASSES}" = "webdoc" ]; then
		#
		# Which case does the CD driver display the filenames on
		# the CD? This call is needed for installing doc, but may
		# be useful for other packages in the future.
		#
		UpperCase

		#
		# The list of files is contained in WEBDOC.lst. Since
		# some platform's drivers display the names of files in
		# uppercase and some in lower case, we need to look 
		# accordingly.
		#
		if [ "${uppercase}" = TRUE ]; then
			webfilelist=`pwd`/WEBDOC.LST
		else
			webfilelist=`pwd`/webdoc.lst
		fi

		#
		# Since the doc is no longer packaged with each
		# platform, make sure the clear text copy from
		# which we are to copy is installed on the CD. It
		# may be the case that for some loads it is not.
		#
		DocOnCD 
		if [ "$?" -eq 1 ]; then
			echo "ERROR: Documentation is not on the CD." >&2
			echo "Please contact customer support for assistance." >&2
			doExit 5
		fi


		#
		# Everything looks ok, so let's install keeping track of
		# our current location so we can return when we finish.
		#
		cwd=`pwd`
		cd $CDROOT
		echo "\nInstalling webdoc..." >&2

		#
		# Copy only those files in the webfilelist to DOCDIR.
		#
		if [ "${uppercase}" = FALSE ]; then
			#
			# Remove any old copies of doc files in DOCDIR 
			# before attempting to install.
			#
			rm -rf $DOCDIR/doc 2>/dev/null

			#
			# Everything is okay, go ahead and install.
			#
			cat $webfilelist | cpio -dumpv $DOCDIR 2>/dev/null
		else
			#
			# Remove any old copies of doc files in DOCDIR 
			# before attempting to install.
			#
			rm -rf $DOCDIR/doc 2>/dev/null

			#
			# Everything looks okay, go ahead and install.
			#
			cat $webfilelist | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' | cpio -dumpv $DOCDIR 2>/dev/null
		fi

		if [ "$?" -ne 0 ] ; then
			echo "ERROR: Problem unloading webdoc..." >&2
			echo "Please contact customer support for assistance" >&2
			doExit 1
		fi
		#
		# Go back to the directory we were in previously.
		#
		cd $cwd
	else

		#
		# Based on CLASSES, unload files.
		# Class name is the same as file name, except for case, plus
		# .Z suffix and possible version number.
		#
		# NOTE: Do not use ranges ([A-Z][a-z]) in the tr statement, 
		# since its behavior is directly related to the machine's 
		# locale.
		# 
		for i in *.[Zz]* ; do
			class=`echo $i | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz' | sed 's/\.z.*$//'`
			for j in $CLASSES ; do
				if [ "$j" = "$class" ] ; then
					( unloadFile `pwd`/$i $DOT ) || exit $?
					break
				fi
			done
		done
	fi


	eval $POSTINSTALL

	if [ "$INSTALLMSGSENT" != "yes" ] ; then
		echo "\nInstallation of "$NAME" was successful" >&2
	fi
	return 0
}


#
# installPkgs: install all packages named in PKGS (from promptPkgs)
#

installPkgs()
{
	for PKG in $PKGS ; do
		# Tack on single quotes
		quoted=\'$PKG\'
		# Get real directory name
		dir=`egrep "^PKG=$1$|^PKG=$quoted$" */[Pp][Kk][Gg][Ii][Nn][Ff][Oo]* /dev/null 2>/dev/null | cut -d/ -f1`
		if [ -z "$dir" -o ! -d "$dir" ] ; then
			echo "ERROR: Can't find directory containing package $1" >&2
			echo "Please contact customer support for assistance" >&2
			doExit 3
		fi

		PREINSTALL=request${PKG}
		POSTINSTALL=postinstall${PKG}

		installPkg "$PKG" "$dir" || doExit $?
		cd $STARTDIR
	done
	return 0
}

#
# valDIR: validate the proposed directory, passed in $1
# Return 0 if ok, 1 if bad.
#

valDIR()
{

	dir=$1
	created=

	#
	# If directory exists, make sure it's writable, searchable,
	# preferably empty.
	#

	if [ -d "$dir" ] ; then
		if [ ! -w "$dir" ] ; then
			echo "ERROR: $dir is not writable; please choose another directory" >&2
			return 1
		fi
		if [ ! -x "$dir" ] ; then
			echo "ERROR: Can't cd to $dir; please choose another directory" >&2
			return 1
		fi
#		if [ "`ls -a $dir | wc -l`" -ne 2 ] ; then
#			echo "Note that directory $dir is not empty" >&2
#			ANSWER=`ckyorn \
#				-h "This installation may fail if files in this directory\nor its subdirectories can't be overwritten!" \
#				-p "Do you wish to choose another directory"` || doExit $?
#			case "$ANSWER" in
#			y|Y|yes|YES)	return 1
#					;;
#			n|N|no|NO)	;;
#			esac
#		fi

	#
	# No good if it's an existing file
	#

	elif [ -f "$dir" ] ; then
		echo "ERROR: $dir is a file" >&2
		return 1

	#
	# Try to make the directory if it does not exist
	# Can't make more than 1 level of pathname - painful to back out
	#

	else
		mkdir $dir 2>/dev/null
		if [ "$?" -ne 0 ] ; then
			echo "ERROR: Can't create $dir; please choose another directory" >&2
			return 1
		fi
		echo "Creating $dir" >&2
		created=yes
	fi

	#
	# Verify cd; pathname must already be absolute (ckpath)
	#
	(
		cd $dir 2>/dev/null
		rtn=$?
	)

	if [ "$rtn" -ne 0 ] ; then
		echo "ERROR: Can't cd to $dir; please choose another directory" >&2
		return 1
	fi

	#
	# See if there's enough space
	#
	checkSpace $dir
	if [ $? -ne 0 ] ; then
		if [ -n "$created" ] ; then
			rmdir $dir
		fi
		return 1
	fi

	return 0
}

#
# Prompts the user with the string given, and
# passes back TRUE for an answer of "y", FALSE for "n" and
# quits with an answer of "q." Prompt_User will continue
# to prompt until the user enters y, n or q.
#
Prompt_User()
{
	#
	# prompt until the user gives a valid response.
	#
	ANSWER=
	while [ -z "${ANSWER}" ]; do
	        echo "$1 [y/n/q]: \c" >&2
		read ANSWER
	        #
	        # Act according to the user's response.
	        #
	        case "${ANSWER}" in
	        y) 
			return 0     # TRUE
	        ;;
	        n) 
			return 1     # FALSE
	        ;;
	        q) 
			doExit 0
	        ;;
	        #
	        # An invalid choice was entered, reprompt.
	        #
	        *) ANSWER=
	        ;;
	        esac
	done
}


#
# Print the available platforms into a readable form.
#
PresentPlatforms()
{
	#
	# Format of CD is MNEMONIC/OSLEVEL. This shell is assumed
	# to be in the root directory. Display the list of directories
	# present.
	#
	getplatforms | \
	awk '
		#
		# Each record is separated by \n, 
		# set the record seperator, RS, appropriately.
		#
		BEGIN { 
			print; 
			RS="\n"
		}
		#
		# Print 3 directories across the screen before a
		# newline is printed.
		#
		{
		if ( NR % 3 == 0 )
			printf "%02d) %-17s\n", NR, $1
		else
			printf "%02d) %-17s", NR, $1;
		}
		END { 
			printf "\n"
		}
	'
}

#
# Determine if the number the user entered exceeds the total 
# number of platforms.
#
ValidPlatformNumber()
{
	if [ "${PLATFORM_NUMBER}" -le "${TOTAL_PLATFORMS}" ]
	then
		#
		# A valid number was entered, return TRUE.
		#
		return 0;
	else
		#
		# An invalid platform number was entered,
		# return FALSE.
		#
		return 1;
	fi
}

#
# Prints an error message.
#
BadPlatformMessage()
{
	echo >&2
	echo "** Invalid entry! Only integer values between 01 and ${TOTAL_PLATFORMS} are legal. **" >&2
}

#
# Get the user's choice for the installation platform.
#
GetPlatformNumber()
{
	#
	# Get the total number of platforms available.
	#
	TOTAL_PLATFORMS=`getplatforms | wc -l`

	PLATFORM_NUMBER=
	#
	# Continually prompt until a valid number is entered.
	#
	while [ -z "${PLATFORM_NUMBER}" ]
	do
		echo >&2
		echo "Install which platform's files? [01-${TOTAL_PLATFORMS}, q to quit, l for list]: \c" >&2
		read PLATFORM_NUMBER
		#
		# Make sure a platform number between 01 and the
		# total number of platforms was entered. If not,
		# reprompt...
		#
		case "${PLATFORM_NUMBER}" in
		#
		# If a number was entered, check to make sure it doesn't 
		# exceed the total number of platforms available. Want to 
		# the possibility of 00 being entered.
		#
		[1-9] | [0-9][1-9] | [1-9][0-9])
			ValidPlatformNumber && break
			BadPlatformMessage
			PLATFORM_NUMBER=
		;;
		#
		# Allow the user to quit.
		#
		q|Q|quit|Quit|QUIT)
			doExit 1
		;;
		#
		# In the case of a carriage return, reprompt.
		#
		"")
			PLATFORM_NUMBER=
		;;
		#
		# Display the available platforms.
		#
		l)
			PresentPlatforms
			PLATFORM_NUMBER=
		;;
		#
		# Anything other than a number is invalid for
		# sure, reprompt.
		#
		*) 
			BadPlatformMessage
			PLATFORM_NUMBER=
		;;
		esac
	done
}

#
# Convert the number chosen to the platform's directory. 
#
ConvertNumberToDirectory()
{
	#
	# Since we don't have arrays in Bourne Shell, this is sort
	# of a kludge.
	#
	i=1
	#
	# Loop until the number chosen is hit.
	#
	for PLATFORM in `getplatforms`
	do
		if [ "$i" -eq "${PLATFORM_NUMBER}" ]; then
			break;
		fi
		i=`expr $i + 1`
	done
}

#
# Display the platform information in the readme file
# located in the directory chosen. 
#
DisplayChoice()
{
	echo >&2
	echo "** You have chosen to install from ${PLATFORM} **" >&2
	echo >&2
	#
	# Readme file may be in upper or lower case.
	#
	if [ -r "${PLATFORM}/README" ]; then
		cat ${PLATFORM}/README >&2
		echo >&2
	else
		if [ -r "${PLATFORM}/readme" ]; then
			cat ${PLATFORM}/readme >&2
			echo >&2
		else
			#
			# Should never get in here - a readme
			# file should be generated for each platform.
			#
			echo >&2
			echo "** ERROR: No platform information exists \c" >&2
			echo "for this selection. **"  >&2
			doExit 1
		fi
	fi
}	

createSuminfofile()
{
 
	###
	### Accept suffix ($2) in this routine to allow processing of
	### different SUMINFO files for each class.
	###
        suminfofile=`echo [Ss][Uu][Mm][Ii][Nn][Ff][Oo]$2`
 
        #
        # Make sure the file exists.
        #
        if [ ! -f "$suminfofile" ]; then
                echo "WARN: Can't create suminfo file" >&2
                return
        fi
 
        #
        # Put the sum information into the filename passed.
        #
	sumfilename=$1
        rm -f $sumfilename 2>/dev/null
 
        cp  $suminfofile $sumfilename
 
        if [ $? -ne 0 ] ; then
                echo "WARN: Can't create suminfo file" >&2
                return
        fi
}

#
# Platform specific functions:
# The purpose of these functions is to
# set any environment specific issues for the install shell.
#
#	Define getFreeBlks function
#	Define ARCH environment variable
#	Set PATH
#	Set any path for the dynamic loader/linker
#
envAlpha()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed 1d | awk '{print $4}'
	}
	ARCH=alpha
}

envGeneric()
{
	# used for all platforms
	PATH=:/usr/5bin:/bin:/usr/bin:$PATH
	export PATH
	getFreeBlks() {
		t=$1
		while true ; do
			t1=`df $t 2>/dev/null`
			if [ $? -eq 0 ] ; then
				echo $t1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
				break
			fi
			t=`dirname $t`
		done
	}
	ARCH=generic
}

envHp()
{
	PATH=:/bin:/usr/bin:$PATH
	export PATH
	getFreeBlks() {
		t=$1
		while true ; do
			t1=`df $t 2>/dev/null`
			if [ $? -eq 0 ] ; then
				echo $t1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
				break
			fi
			t=`dirname $t`
		done
	}
	ARCH=9000
	#
	# _TMLDPATH - shared library path if not LD_LIBRARY_PATH
	#
	_TMLDPATH=SHLIB_PATH
}

envIbm()
{
	PATH=:/usr/bin:$PATH
	export PATH
	getFreeBlks() {
		df $1 | awk '{ if (NR==2) printf "%d\n", $3 * 2 }'
	}
	ARCH=rs6000
	#
	# _TMLDPATH - shared library path if not LD_LIBRARY_PATH
	#
	_TMLDPATH=LIBPATH
}

envNcr()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=i386
}

envUnisys()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=i386
}

envPyr()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=MIServer-S
}

envSni()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=mips
}

envTandem()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=mips-S
}

envSco()
{
	PATH=:/bin:/usr/bin:$PATH
	export PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=i386
}

envSun4x()
{
	PATH=:/usr/5bin:/bin:/usr/bin::/usr/ccs/bin:/usr/ucb:$PATH
	export PATH
	getFreeBlks() {
		df -t $1 | sed -e 1d | awk '{print $2}'
	}
	ARCH=sun4
}

envSun5x()
{
	PATH=:/usr/5bin:/bin:/usr/bin:/usr/ucb:/usr/ccs/bin:$PATH
	export PATH
	getFreeBlks() {
		df -t $1 | sed -e 2d -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=sparc
}

envUw()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=i386
}

envDgintl()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[   ]*//' -e 's/ blocks.*//'
	}
	ARCH=i386
}

envDgm88k()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[   ]*//' -e 's/ blocks.*//'
	}
	ARCH=mc88110
}

envSeq()
{
	PATH=:/bin:/usr/bin:$PATH
	export PATH
	getFreeBlks() {
		df $1 | sed -e 's/.*):[ 	]*//' -e 's/ blocks.*//'
	}
	ARCH=sequent
}

envSgi()
{
	PATH=:/bin:/usr/bin/:$PATH
	getFreeBlks() {
		df $1 | sed -e 1d | awk '{print $5}'
	}
	ARCH=sgi
}

# End of generic functions
##########################################
#
#
# APPLICATION SPECIFIC FUNCTIONS
#
#
##########################################

#
# Generate a list of platforms that can be installed by examining the names
# of the directories below us and eliminating both the non-Unix and
# documentation directories.
#
getplatforms()
{
	ls -d */* 2>/dev/null | sed -e '/dos/d' -e '/os232/d' -e '/win16/d' -e '/winnt/d' -e '/alphant/d' -e '/mac/d' -e '/win95/d' -e '/doc/d' -e '/as400/d' -e '/ibmoe/d' -e '/DOS/d' -e '/OS232/d' -e '/WIN16/d' -e '/WINNT/d' -e '/ALPHANT/d' -e '/MAC/d' -e '/WIN95/d' -e '/DOC/d' -e '/AS400/d' -e '/IBMOE/d'
}

#
# promptPkgs: prompt for packages to install
# Mimics pkgadd behavior
# Sets PKGS to list of packages to install.
# Sets ALLPKGS to non-null if all was specified.
# Returns 0 if ok, otherwise failed.
#

promptPkgs()
{

	# Get prompt strings by looking for PKGINFO file in each subdir
	param1=
	param2=
	param3=
	param4=
	pkg1=
	pkg2=
	pkg3=
	pkg4=
	for i in * ; do
		pkginfo=`echo $i/[Pp][Kk][Gg][Ii][Nn][Ff][Oo]*`
		if [ -r "$pkginfo" ] ; then
			pkg="`grep '^PKG=' $pkginfo | sed 's/PKG=//'`"
			pkgname="`grep '^NAME=' $pkginfo | sed 's/NAME=//'`"
		else
			continue
		fi

		# pkg, pkgname may have quotes, so get rid of them
		pkg=`echo $pkg | sed "s/'//g"`
		pkgname=`echo $pkgname | sed "s/'//g"`

		# Based on the pkg, determine how many there are.
		case "$pkg" in
		tuxcore|iceberg|enc128|enc40|xgui|tps|msgq|mqsc)
			param1="$pkg	$pkgname"
			pkg1="$pkg"
			;;
		tuxdoc|joltdoc|msgqdoc|mqscdoc)
			param2="$pkg	$pkgname"
			pkg2="$pkg"
			;;
		jolt)
			param3="$pkg	$pkgname"
			pkg3="$pkg"
			;;
		esac
	done

	ANSWER=`ckitem -l "The following packages are available:" \
		-p "Select the package(s) you wish to install (or 'all' to install\nall packages)" \
		-i all -d all \
		"$param1" "$param2" "$param3" "$param4" "all	all"`
	rtn=$?
	if [ $rtn -ne 0 ] ; then
		if [ $rtn -eq 3 ] ; then
			echo "\nInstallation terminated" >&2
			exit 0
		fi
		return $rtn
	fi

	# If all, go back and get the packages in the same order as presented
	if [ "$ANSWER" = all ] ; then
		ALLPKGS=all
		all=
		for i in "$pkg1" "$pkg2" "$pkg3" "$pkg4" ; do
			all="$all $i"
		done
		ANSWER="`echo $all`"
	fi
	PKGS="$ANSWER"
	return 0
}

#
# promptMQOpts: prompt for software installation options
# Set WSONLY and CLASSES
#

promptMQOpts()
{

	# First we ask which version of Tux (if any) they want to use.
	if [ "$supportsM3" = y ]
	then
        	ANSWER=`ckitem -l "The following connectivity options are available:" \
                	-p "Select an option" \
                	-d "none" \
                	"tux64	Install On Top Of BEA TUXEDO v6.4" \
			"m3_21	Install On Top Of BEA M3 v2.1" \
                	"none	Install Without BEA TUXEDO or BEA M3"` || doExit $?
	else
        	ANSWER=`ckitem -l "The following connectivity options are available:" \
                	-p "Select an option" \
                	-d "none" \
                	"tux64	Install On Top Of BEA TUXEDO v6.4" \
                	"none	Install Without BEA TUXEDO"` || doExit $?
	fi

        # Set CLASSES to the chosen option.  This will be combined with a
	# choice from below.
        case "$ANSWER" in
        tux64)	CLASSES='mqtux4'
                ;;
        m3_21)	CLASSES='mqtux5'
		;;
	none)	CLASSES='mqdevx'
		;;
        *)      doExit 1
                ;;
        esac

	# Now find out what MessagQ components they want installed
        ANSWER=`ckitem -l "The following installation options are available:" \
                -p "Select an option" \
                -d "both" \
                "both	Server and client" \
		"client	Client only" ` || doExit $?

        # Make sure correct CLASSES are known to checkSPACE
        case "$ANSWER" in
        both)   CLASSES="mqdev mqcl mqexa $CLASSES"
                ;;
        client) WSONLY=yes
                CLASSES="mqcl mqexa $CLASSES"
		;;
        *)      doExit 1
                ;;
        esac

}

#
# promptTUXOpts: prompt for software installation options
# Set WSONLY and CLASSES
#

promptTUXOpts()
{

	ANSWER=`ckitem -l "The following installation options are available:" \
		-p "Select an option" \
		-d "both" \
		"both	Install the full TUXEDO Core System - client and server" \
		"client	Install the client only"` || doExit $?

	# Make sure correct CLASSES are known to checkSPACE
	case "$ANSWER" in
	both)	CLASSES='ws wsc wsc1 wsc2 wsc3'
		;;
	client)	WSONLY=yes
		CLASSES='ws'
		;;
	*)	doExit 1
		;;
	esac

}

#
# promptICEOpts: prompt for software installation options
# Set WSONLY and CLASSES
#

promptICEOpts()
{

	ANSWER=`ckitem -l "The following installation options are available:" \
		-p "Select an option" \
		-d "both" \
		"both	Install the full Millennia3 System - client and server" \
		"client	Install the client only"` || doExit $?

	# Make sure correct CLASSES are known to checkSPACE
	case "$ANSWER" in
	both)	CLASSES='ws wsc wsc1 wsc2 wsc3'
		;;
	client)	WSONLY=yes
		CLASSES='ws'
		;;
	*)	doExit 1
		;;
	esac

}

#

#
# promptDOCOpts: prompt for documentation installation options
# Set CLASSES and WEBDOCONLY
#

promptDOCOpts()
{

	WEBDOCONLY=

	ANSWER=`ckitem -l "The following installation options are available:" \
		-p "Select an option" \
		-d "alldoc" \
		"alldoc	Install the complete TUXEDO Online Documentation Set" \
		"webdoc	Install the Web Admin GUI Documentation only"` || doExit $?

	# Make sure correct CLASSES are known to checkSPACE
	case "$ANSWER" in
	alldoc)	CLASSES='tuxdoc'
		;;
	webdoc)	CLASSES='webdoc'
		WEBDOCONLY=yes
		;;
	*)	doExit 1
		;;
	esac

}

#
# promptDOCDIR: prompt for and validate base directory for online doc
#

promptDOCDIR() {

	if [ "$WEBDOCONLY" != yes ] ; then
		while true ; do
			ANSWER=`ckpath -a \
			-d $1 \
			-p "Base directory for documentation installation"` || doExit $?
			valDIR "$ANSWER"
			if [ "$?" -eq 0 ] ; then
				break
			fi
		done
		DOT=$dir
		DOCDIR=$dir
		echo "\nUsing $DOT for the documentation base directory" >&2
	else
		DOT=$1
		DOCDIR=$DOT
	fi
}

promptJOLTDOCDIR() {

	while true ; do

		ANSWER=`ckpath -a \
		-d $1 \
		-p "Base directory for JOLT documentation installation"` || doExit $?
	valDIR "$ANSWER"

	if [ "$?" -eq 0 ] ; then
		break
	fi

	done

	DOT=$ANSWER
	JOLTDOCDIR=$dir

	echo "\nUsing $DOT for the JOLT documentation base directory" >&2
}

#
# promptWEBDIR: prompt for and validate WEBDIR
#

promptWEBDIR()
{

	if [ "$WSONLY" != yes ] ; then
		# Make sure the directory one level above the suggested
		# default exists.
		mkdir $TUXDIR/udataobj >/dev/null 2>&1
		while true ; do
	
			ANSWER=`ckpath -a \
				-d "$TUXDIR/udataobj/webgui" \
				-p "Directory where TUXEDO web GUI documents and images are to be installed" \
				-h "Consult your web server documentation for an appropriate directory"` || doExit $?
	
			valDIR "$ANSWER"
			if [ "$?" -eq 0 ] ; then
				break
			fi
		done
		# Pick up dir, since it may be different from ANSWER
		WEBDIR=$dir
		export WEBDIR
		DOT=$dir
		echo "\nUsing $WEBDIR as the TUXEDO web GUI document tree" >&2
	fi
}

#
# promptWEBCGIDIR: prompt for and validate WEBCGIDIR
#

promptWEBCGIDIR()
{

	if [ "$WSONLY" != yes ] ; then
		while true ; do
	
			ANSWER=`ckpath -a \
				-d "$WEBDIR/cgi-bin" \
				-p "Directory where TUXEDO web GUI CGI programs are to be installed" \
				-h "Consult your web server documentation for an appropriate CGI directory"` || doExit $?
	
			valDIR "$ANSWER"
			if [ "$?" -eq 0 ] ; then
				break
			fi
		done
		# Pick up dir, since it may be different from ANSWER
		WEBCGIDIR=$dir
		export WEBCGIDIR
		DOT=$dir
		echo "\nUsing $WEBCGIDIR as the TUXEDO web GUI CGI directory" >&2
	fi
}

#
# promptWEBCGIPREFIX: prompt for and validate WEBCGIPREFIX
#

promptWEBCGIPREFIX()
{

	if [ "$WSONLY" != yes ] ; then
		while true ; do
	
			ANSWER=`ckstr \
				-r '/.*[^/]' \
				-d "/cgi-bin" \
				-p "Web server client prefix for CGI directory.  /cgi-bin is a good choice for most web servers." \
				-e "Prefix must start with /, must have at least one additional character, and must not end with /." \
				-h "Consult your web server documentation for an appropriate CGI prefix"` || doExit $?
	
			if [ "$?" -eq 0 ] ; then
				break
			fi
		done
		WEBCGIPREFIX=$ANSWER
		export WEBCGIPREFIX
		echo "\nUsing $WEBCGIPREFIX as the TUXEDO web GUI CGI prefix" >&2
	fi
}

#
# promptWEBJAVADIR: prompt for and validate WEBJAVADIR
#

promptWEBJAVADIR()
{

	if [ "$WSONLY" != yes ] ; then
		while true ; do
	
			ANSWER=`ckpath -a \
				-d "$WEBDIR/java" \
				-p "Directory where TUXEDO web GUI java applets are to be installed" \
				-h "Consult your web server documentation for an appropriate JAVA directory"` || doExit $?
	
			valDIR "$ANSWER"
			if [ "$?" -eq 0 ] ; then
				break
			fi
		done
		# Pick up dir, since it may be different from ANSWER
		WEBJAVADIR=$dir
		export WEBJAVADIR
		DOT=$dir
		mkdir $WEBJAVADIR >/dev/null 2>&1
		mkdir $WEBJAVADIR/snapshot >/dev/null 2>&1
		echo "\nUsing $WEBJAVADIR as the TUXEDO web GUI document tree" >&2
	fi
}

#
# promptBEADIR:
# if BEADIR is not already set, and TUXDIR is not already set, prompt
# for and validate a directory into which we will install MessageQ.
#

promptBEADIR()
{

	if [ -n "$BEADIR" ] ; then
        	DOT=$BEADIR
	else
		if [ -n "$TUXDIR" ] ; then
			DOT=$TUXDIR
		fi
	fi

        while true ; do

                ANSWER=`ckpath -a \
                        -p "Directory where MessageQ files are to be installed" \
			-d "$DOT" \
                        -h "Specify a directory for the MessageQ base directory"` || doExit $?

                valDIR "$ANSWER"
                if [ "$?" -eq 0 ] ; then
                        break
                fi
        done
        # Pick up dir, since it may be different from ANSWER
        BEADIR=$dir
        export BEADIR
        DOT=$dir
        echo "\nUsing $BEADIR as the MessageQ base directory" >&2

}

#
# promptExistTUXDIR: prompt for and validate an existing (previously installed)
# Tuxedo installation.  The version portion of the prompt is passed to this
# function as an argument.
#
promptExistTUXDIR()
{

	# Loop until we get a good TUXDIR
	while true ; do

		while true ; do

		    if [ "$supportsM3" = y ]
		    then
			ANSWER=`ckpath -a \
				-p "Directory where $1 files are installed" \
				-d "$TUXDIR" \
				-h "Specify the directory where TUXEDO or M3 has been installed"` || doExit $?
		    else
			ANSWER=`ckpath -a \
				-p "Directory where $1 files are installed" \
				-d "$TUXDIR" \
				-h "Specify the directory where TUXEDO has been installed"` || doExit $?
		    fi

			valDIR "$ANSWER"
			if [ "$?" -eq 0 ] ; then
				break
			fi
		done

		#
		# libtux.a is installed on every TUXEDO platform.
		#
		if [ ! -f "$ANSWER/lib/libtux.a" ]; then
			echo "\nERROR: $ANSWER doesn't contain a valid $1 installation (cannot read $ANSWER/lib/libtux.a).\n" >&2
		else
			break
		fi

	done

	# Pick up dir, since it may be different from ANSWER
	TUXDIR=$dir
	export TUXDIR
	BEADIR=$TUXDIR
	export BEADIR
	DOT=$dir
	echo "\nUsing $TUXDIR as the installation base directory" >&2
}

#
# promptTUXDIR: prompt for and validate TUXDIR
#

promptTUXDIR()
{

	while true ; do

		ANSWER=`ckpath -a \
			-p "Directory where TUXEDO files are to be installed" \
			-h "Specify a directory for the TUXEDO base directory"` || doExit $?

		valDIR "$ANSWER"
		if [ "$?" -eq 0 ] ; then
			break
		fi
	done
	# Pick up dir, since it may be different from ANSWER
	TUXDIR=$dir
	export TUXDIR
	DOT=$dir
	echo "\nUsing $TUXDIR as the TUXEDO base directory" >&2
}

#
# promptICEDIR: prompt for and validate TUXDIR
#

promptICEDIR()
{

	while true ; do

		ANSWER=`ckpath -a \
			-p "Directory where Millennia3 files are to be installed" \
			-h "Specify a directory for the Millennia3 base directory"` || doExit $?

		valDIR "$ANSWER"
		if [ "$?" -eq 0 ] ; then
			break
		fi
	done
	# Pick up dir, since it may be different from ANSWER
	TUXDIR=$dir
	export TUXDIR
	DOT=$dir
	echo "\nUsing $TUXDIR as the Millennia3 base directory" >&2
}

getTUXDIR()
{

	while true ; do

		ANSWER=`ckpath -a \
			-p "Location of Millennia3 2.1 installation" \
			-h "Specify the Millennia3 2.1 base directory"` || doExit $?

		#
		# libgp.a is installed on every TUXEDO platform.
		#
		if [ ! -f "$ANSWER/lib/libgp.a" ]; then
			echo "\nERROR: $ANSWER doesn't contain a valid TUXEDO installation.\n" >&2
			doExit 1
		fi

		#
		# Depending upon the platform, the shared gp library could
		# be named libgp.sl, libgp.so, libgp.so.60, libgp.so.6.0,
		# or libgp.a (on AIX).
		#
		# To prevent the need for the user to re-archive if
		# they choose to revert back to a previous encryption level,
		# create a backup of the libgp.a file as well (assuming the
		# platform is not AIX).
		#
		GPFILENAME=
		GPFILENAMEA=
		for file in `ls $ANSWER/lib/libgp.*`
		do
			case $file in
        		*libgp.sl|*libgp.so.60|*libgp.so|*libgp.so.6.0) 
				GPFILENAME=`basename $file`
				;;
			*libgp.a)
				GPFILENAMEA=`basename $file`
				;;
			esac
		done

		# GPFILENAME will be null if the platform is AIX.
		if [ -z "$GPFILENAME" ] ; then
			GPFILENAME=$GPFILENAMEA
		fi

		valDIR "$ANSWER"

		if [ "$?" -eq 0 ] ; then
			break
		fi

	done
	#
	# Pick up dir, since it may be different from ANSWER
	#
	TUXDIR=$dir
	export TUXDIR
	DOT=$dir
	export GPFILENAME GPFILENAMEA
	echo "\nUsing $TUXDIR as the TUXEDO base directory" >&2
}

promptTPSDIR()
{

	while true ; do

		ANSWER=`ckpath -a \
			-p "Directory where TPS files are to be installed" \
			-h "Specify a directory for the TPS base directory"` || doExit $?

		valDIR "$ANSWER"
		if [ "$?" -eq 0 ] ; then
			break
		fi
	done

	#
	# Pick up dir, since it may be different from ANSWER
	#
	TPSDIR=$dir
	export TPSDIR
	DOT=$dir

	echo "\nUsing $TPSDIR as the TPS base directory" >&2
}

promptXGUIDIR()
{

	while true ; do

		ANSWER=`ckpath -a \
			-p "Base directory for the X-GUI files" \
			-h "This directory must be the same as <TUXDIR> (you will need write permission)"` || doExit $?

		valDIR "$ANSWER"
		if [ "$?" -eq 0 ] ; then
			break
		fi
	done

	#
	# Pick up dir, since it may be different from ANSWER
	#
	XGUIDIR=$dir
	export XGUIDIR
	DOT=$dir

	echo "\nUsing $XGUIDIR as the X-GUI base directory" >&2
}

#
# newTUXLocale: set up symbolic links if LANG!=C
# $* is list of full pathnames of candidate C directories
#

newTUXLocale()
{

	iLANG=$LANG

	if [ -z "$iLANG" -o "$iLANG" = C ] ; then
		return
	fi

	# We will be changing the current directory.  Save the name so we can
	# get back to it.

	InstDir=`pwd`

	for i in $* ; do
		dn=`dirname $i 2>/dev/null`
		if [ -z "$dn" -o -d "$dn/$iLANG" ] ; then
			continue
		fi
		echo "\nSetting up additional locale: $dn/$iLANG ..." >&2
		cd $dn
		if [ $? -ne 0 ] ; then
			echo "ERROR: Can't cd to $dn" >&2
			echo "Please contact customer support for assistance" >&2
			doExit 3
		fi
		ln -s C $iLANG
		if [ $? -ne 0 ] ; then
			echo "ERROR: Can't link $dn/C to $dn/$iLANG" >&2
			echo "Please contact customer support for assistance" >&2
			doExit 3
		fi
		echo "... finished" >&2
	done

	# Return to the installation directory so we can continue.

	cd $InstDir
}

#
# finisMsg: Output registration message.
#

finisMsg()
{

	echo "\nPlease don't forget to fill out and send in your registration card" >&2
}


#
# createTUXwsvrIni: Create $TUXDIR/udataobj/tuxwsvr.ini file for use
# with the tuxwsvr generic web server
#

createTUXwsvrIni()
{

	if [ "$WSONLY" != yes ] ; then
		if [ -d "$TUXDIR" -a -d "$WEBDIR" ]
		then
			WINI=$TUXDIR/udataobj/tuxwsvr.ini
			echo "# tuxwsvr initialization file." >$WINI
			echo "# Created `date` by TUXEDO System installation program." >>$WINI
			echo "#" >>$WINI
			echo "CGI	$WEBCGIPREFIX	$WEBCGIDIR" >>$WINI
			echo "HTML	/java	$WEBJAVADIR" >>$WINI
			echo "HTML	/doc	${DOCDIR:-$TUXDIR/doc}" >>$WINI
			echo "HTML	/	$WEBDIR" >>$WINI
			chmod 644 $WINI
		fi
	fi
}

#
# createWebGuiIni: Create $TUXDIR/udataobj/webgui/webgui.ini file for use
# with the web GUI.
#

createWebGuiIni()
{

	if [ "$WSONLY" != yes ] ; then
	
		if [ -d "$TUXDIR" -a -d "$WEBDIR" ]
		then
			WINI=$TUXDIR/udataobj/webgui/webgui.ini
			echo "# Web GUI initialization file." >$WINI
			echo "# Created `date` by TUXEDO System installation program." >>$WINI
			echo "#" >>$WINI
			echo "TUXDIR=$TUXDIR" >>$WINI
			echo "INIFILE=$TUXDIR/udataobj/webgui/webgui.ini" >>$WINI
			echo "NADDR=//`uname -n`:4003" >>$WINI
			if [ -r /dev/tcp ]
			then
				echo "DEVICE=/dev/tcp" >>$WINI
			else
			if [ -r /dev/inet/tcp ]
			then
				echo "DEVICE=/dev/inet/tcp" >>$WINI
			else
			if [ -r /dev/it ]
			then
				echo "DEVICE=/dev/it" >>$WINI
			else
			if [ -r /dev/streams/xtiso/tcp ]
			then
				echo "DEVICE=/dev/streams/xtiso/tcp" >>$WINI
			else
			if [ -r /patches/tcp ]
			then
				echo "DEVICE=/patches/tcp" >>$WINI
			fi
			fi
			fi
			fi
			fi
			echo "CODEBASE=/java" >>$WINI
			echo "DOCBASE=/doc" >>$WINI
			echo "SNAPDIR=$WEBJAVADIR/snapshot" >>$WINI
			echo "SNAPBASE=/java/snapshot" >>$WINI
			echo "#" >>$WINI
			echo "# In order to configure one or more domains as part of the Web GUI pull-down" >>$WINI
			echo "# menu, add lines to this file of the form DOMAIN=domainname;tuxconfig" >>$WINI
			chmod 644 $WINI
		fi
	fi
}

#
# createTUXenvfile: Create tux.env file containing useful environment variable
# settings
#

createTUXenvfile()
{

	TUXENVFILE=$1/tux.env
	if [ -f "$TUXENVFILE" ] ; then
		chmod u+w $TUXENVFILE 2>/dev/null || return
		if [ ! -w "$TUXENVFILE" ] ; then
			return
		fi
	fi

	case "$PKG" in
	tuxcore)
		cat <<! >> $TUXENVFILE
TUXDIR=$DOT; export TUXDIR
PATH=\$TUXDIR/bin:\$PATH; export PATH
COBCPY=:\$TUXDIR/cobinclude; export COBCPY
COBOPT="-C ANS85 -C ALIGN=8 -C NOIBMCOMP -C TRUNC=ANSI -C OSEXT=cbl"; export COBOPT
!
		if [ -f $TUXDIR/lib/libgp.so* -o -f $TUXDIR/lib/libgp.sl -o -n "$_TMLDPATH" ] ; then
			if [ -z "$_TMLDPATH" ] ; then
				_TMLDPATH=LD_LIBRARY_PATH
			fi
			cat <<! >> $TUXENVFILE
$_TMLDPATH=\$TUXDIR/lib:\$$_TMLDPATH; export $_TMLDPATH
!
		fi
		;;

	esac

}

#
# createICEenvfile: Create mil3.env file containing useful environment variable
# settings
#

createICEenvfile()
{

	ICEENVFILE=$1/mil3.env
	if [ -f "$ICEENVFILE" ] ; then
		chmod u+w $ICEENVFILE 2>/dev/null || return
		if [ ! -w "$ICEENVFILE" ] ; then
			return
		fi
	fi

	case "$PKG" in
	iceberg)
		cat <<! > $ICEENVFILE
TUXDIR=$DOT; export TUXDIR
PATH=\$TUXDIR/bin:\$PATH; export PATH
COBCPY=:\$TUXDIR/cobinclude; export COBCPY
COBOPT="-C ANS85 -C ALIGN=8 -C NOIBMCOMP -C TRUNC=ANSI -C OSEXT=cbl"; export COBOPT
WEBJAVADIR=$WEBJAVADIR
!
		if [ -f $TUXDIR/lib/libgp.so* -o -f $TUXDIR/lib/libgp.sl -o -n "$_TMLDPATH" ] ; then
			if [ -z "$_TMLDPATH" ] ; then
				_TMLDPATH=LD_LIBRARY_PATH
			fi
			cat <<! >> $ICEENVFILE
$_TMLDPATH=\$TUXDIR/lib:\$$_TMLDPATH; export $_TMLDPATH
!
		fi
		;;

	esac

}

#
# Function to make sure the UW version is 2.x
#
checkUWVersion()
{

	echo "Checking UnixWare version..." >&2

	if [ "`uname -v | awk '{print substr($1,1,1)}'`" != "2" ]; then
		echo "\n...ERROR: UnixWare version 2.x is required to install TUXEDO." >&2
		doExit 3
	fi

	echo "... finished" >&2
}

#
# Function which prompts for the tlisten password.
# setTlistenpwd Asumptions
# o We are not installing Workstation only.
# o Requires TUXDIR to be set.
# o Assumes that TUXDIR/udataobj exists.
# o Assumes that the system umask is set properly 600
#
setTlistenPwd()
{

	if [ "$WSONLY" != yes ] ; then
		#
		# udataobj must exist under TUXDIR.
		#
		if [ ! -d $TUXDIR/udataobj ] ; then
			echo "\nERROR: Invalid TUXDIR..." >&2
			doExit 1
		fi

		#
		# Run the utility which prompts for the tlisten
		# password. Set the library path since tlistpwd
		# makes use of shared libraries.
		#
		echo >&2
		eval $_TMLDPATH=$TUXDIR/lib $TUXDIR/bin/tlistpwd $TUXDIR

		if [ $? != 0 ] ; then
			echo "\nERROR: Cannot install tlistener password." >&2
			doExit 1
		fi

		#
		# Permissions should be 600 on the password file.
		# and the password utility should be removed after
		# it is used.
                #              
		chmod 600 $TUXDIR/udataobj/tlisten.pw
		rm -f $TUXDIR/bin/tlistpwd  

	fi
}

#
# createTUXlicFile: Create default empty $TUXDIR/udataobj/lic.txt file.
#

createTUXlicFile()
{

	# Skip this if we are installing /Workstation only.
	if [ "$WSONLY" != yes ] ; then

		# Tell the user what we are doing
		echo
		echo "Processing default license file..." >&2

		# Determine whether or not the license file exists.  If not, create it.
		LIC=$TUXDIR/udataobj/lic.txt
		if [ ! -f "$LIC" ]
		then
			echo "# TUXEDO license file" >$LIC
			echo "" >>$LIC
			echo "# This file contains license tokens to enable TUXEDO and optional components." >>$LIC
			echo "# Each license begins with \"[section name]\" and ends with a \"SIGNATURE=\" line." >>$LIC
			echo "# New license sections should be appended to this file, and the old section," >>$LIC
			echo "# if present, should be deleted." >>$LIC
			echo "" >>$LIC
			echo "# WARNING: altering parameters within a section will invalidate the license." >>$LIC
			echo "# This is a violation of BEA System's licensing agreement, and also may" >>$LIC
			echo "# disable TUXEDO or optional components." >>$LIC
			echo "" >>$LIC
			echo "# For technical support or to obtain a license, call 888-BEA-SUPT" >>$LIC
			echo "# (888-232-7878) or 408-743-4070." >>$LIC
			chmod 644 $LIC
		fi
		echo "... finished" >&2
	fi
}

#
# createMQlicFile: Create default empty $BEADIR/udataobj/lic.txt file.
#

createMQlicFile()
{

        # Skip this if we are installing /Workstation only.
        if [ "$WSONLY" != yes ] ; then

                # Tell the user what we are doing
                echo
                echo "Processing default license file..." >&2

		if [ ! -d  "$BEADIR/udataobj" ] ; then
			mkdir $BEADIR/udataobj
		fi

                # Determine whether or not the license file exists.  If not, create it.
                LIC=$BEADIR/udataobj/lic.txt
                if [ ! -f "$LIC" ]
                then
                        echo "# MessageQ license file" >$LIC
                        echo "" >>$LIC
                        echo "# This file contains license tokens to enable MessageQ and optional components." >>$LIC
                        echo "# Each license begins with \"[section name]\" and ends with a \"SIGNATURE=\" line." >>$LIC
                        echo "# New license sections should be appended to this file, and the old section," >>$LIC
                        echo "# if present, should be deleted." >>$LIC
                        echo "" >>$LIC
                        echo "# WARNING: altering parameters within a section will invalidate the license." >>$LIC
                        echo "# This is a violation of BEA System's licensing agreement, and also may" >>$LIC
                        echo "# disable MessageQ or optional components." >>$LIC
                        echo "" >>$LIC
                        echo "# For technical support or to obtain a license, call 888-BEA-SUPT" >>$LIC
                        echo "# (888-232-7878) or 408-743-4070." >>$LIC
                        chmod 644 $LIC
                fi
                echo "... finished" >&2
        fi
}

#
# remindLicMsg: Display reminder to copy license file from floppy.
#

remindLicMsg()
{

	# Skip this section if we are installing /Workstation only.
	if [ "$WSONLY" != yes ] ; then

		echo "\nPlease don't forget to manually copy the contents of the license file" >&2
		echo "from the enclosed floppy to '$BEADIR/udataobj/lic.txt'." >&2
		echo "Refer to the Installation Guide for details on how to do this.\n" >&2

	fi
}

#
# remindLicUtilMsg: Display reminder to use lic.sh to
# 	 	    copy license file from floppy.
#

remindLicUtilMsg()
{

	# Skip this section if we are installing Client only.
	if [ "$WSONLY" != yes ] ; then

		echo "\nPlease don't forget to use lic.sh located in your Millennia3 bin directory" >&2
		echo "to install the license file from the enclosed floppy." >&2
		echo "Refer to the BEA Millennia3 Installation Guide for details on how to do this.\n" >&2

	fi
}

#
# MessageQ PRE-INSTALLATION SCRIPT.
#
requestmsgq()
{

        WSONLY=
        DOT=
        #
        # If UnixWare, make sure of the version.
        #
        if [ "$MNEMONIC" = "sco_uw" ]; then
                checkUWVersion
        fi

        promptMQOpts

	# Based on the Tuxedo selection, decide which dir we prompt for
	for j in $CLASSES; do
		case "$j" in
		mqtux4)	promptExistTUXDIR "TUXEDO 6.4"
				;;
		mqtux5)	promptExistTUXDIR "BEA M3"
			;;
		mqdevx)	promptBEADIR
			;;
		esac
	done

        DOT="$BEADIR"
        return 0

}

#
# MessageQ POST-INSTALLATION SCRIPT.
#
postinstallmsgq()
{

        createSuminfofile $BEADIR/.msgqsuminfo

	# Append MessageQ entry to locale/CATNAMES
	if [ ! -d ${BEADIR}/locale ] ; then
		mkdir ${BEADIR}/locale
	fi
	catnamesfile=${BEADIR}/locale/CATNAMES
	if [ -f ${catnamesfile} ] ; then
		chmod u+w $catnamesfile
	fi
	echo "45 BMQ_CAT" >> $catnamesfile
	chmod u-w $catnamesfile

        changeBEAPerms $BEADIR/.msgqsuminfo

        createMQlicFile
        remindLicMsg

        return 0
}

#
# MessageQ PRE-INSTALLATION SCRIPT.
#
requestmqsc()
{
        MQVERSION=
        DOT=
	CLASSES=mqsc

        promptMQSCOpts

	promptBEADIR

        DOT="$BEADIR"
        return 0
}

#
# MessageQ POST-INSTALLATION SCRIPT.
#
postinstallmqsc()
{
        createSuminfofile $BEADIR/.mqscsuminfo

        changeBEAPerms $BEADIR/.mqscsuminfo
	if [ "$MQVERSION" = MQV40A ]
	then
		rm -f $BEADIR/bin/qmbsrv
		cp $BEADIR/bin/qmbsrv40 $BEADIR/bin/qmbsrv
		rm -f $BEADIR/bin/qmb_util
		cp $BEADIR/bin/qmb_util40 $BEADIR/bin/qmb_util
	else
		rm -f $BEADIR/bin/qmbsrv
		cp $BEADIR/bin/qmbsrv50 $BEADIR/bin/qmbsrv
		rm -f $BEADIR/bin/qmb_util
		cp $BEADIR/bin/qmb_util50 $BEADIR/bin/qmb_util
	fi

        return 0
}

#
# promptMQSCOpts: prompt for software installation options
#

promptMQSCOpts()
{

	# First we ask which version of MessageQ they want to use.
       	ANSWER=`ckitem -l "The following connectivity options are available:" \
               	-p "Select an option" \
               	-d "none" \
               	"MQ40A	Install On Top Of BEA MessageQ 4.0A" \
		"MQ50	Install On Top Of BEA MessageQ 5.0"` || doExit $?

        case "$ANSWER" in
        MQ40A|MQ50)	MQVERSION=$ANSWER
                ;;
        *)      doExit 1
                ;;
        esac
}
#
# TUXEDO's PRE-INSTALLATION SCRIPT for its Core package.
#
requesttuxcore()
{

	WSONLY=
	TUXDIR=
	DOT=
	#
	# If UnixWare, make sure of the version.
	#
	if [ "$MNEMONIC" = "uw" ]; then
		checkUWVersion
	fi

	promptTUXOpts
	promptTUXDIR
	promptWEBDIR
	promptWEBJAVADIR
	promptWEBCGIDIR
	promptWEBCGIPREFIX

	DOT="$TUXDIR"
	return 0
}

#
# TUXEDO's POST-INSTALLATION SCRIPT for its Core package.
#
postinstalltuxcore()
{

	createTUXenvfile $TUXDIR
	newTUXLocale `find $TUXDIR/locale -type d -name C -print 2>/dev/null`
	createSuminfofile $TUXDIR/.suminfo
	changeTUXPerms $TUXDIR/.suminfo
	#
	# Change permissions before moving web GUI files since
	#  - web GUI files may or may not be installed under TUXDIR
	#  - files that may be modified by the adminstrator have perm 644
	#  - tuxadm may or may not be in a directory with "bin" in the name
	#
	mvWebGuiFiles
	createTUXwsvrIni
	createWebGuiIni
	createTUXlicFile
	setTlistenPwd
	remindLicMsg

	return 0
}

#
# ICEBERG's PRE-INSTALLATION SCRIPT for its Core package.
#
requesticeberg()
{

	WSONLY=
	TUXDIR=
	DOT=
	ERRMSG1=`cat <<!
\nA prior installation of the Millennia3 2.1 full system has been detected.\
\nTo install only the client component, first remove the existing system.
!`

	#
	# If UnixWare, make sure of the version.
	#
	if [ "$MNEMONIC" = "uw" ]; then
		checkUWVersion
	fi

	promptICEOpts
	promptICEDIR
	#
	# Disallow M3 client install over M3 server install
	# If M3 server media intact, and server install initiate reinstall logic
	# If M3 client media intact, and client install initiate reinstall logic
	#
        if [ -f $TUXDIR/.suminfo ] ; then
                if [ "`awk 'NR == 1 {print $2 $5}' "$TUXDIR"/\.suminfo`" = \
                     "MILLENNIA32.1" ] ; then
			# prior server install/client install now selected
			if [ "$WSONLY" = "yes" ] ; then
				echo $ERRMSG1
				doExit 1
			# prior server install/server install now selected
			else
				echo "\nChecking installed media..." >&2
				WEBJAVADIR=`grep WEBJAVADIR $TUXDIR/mil3.env \
					    2>/dev/null|\
					    sed s/WEBJAVADIR=//g` 
				export WEBJAVADIR
				verifyInst $TUXDIR/.suminfo
				ckReinstStatus
			fi
                fi
        elif [ -f $TUXDIR/.clientsuminfo -a "$WSONLY" = "yes" ] ; then
		echo "\nChecking installed media..." >&2
		verifyInst $TUXDIR/.clientsuminfo
		ckReinstStatus
	fi
	#
	# SKIPINST = "yes" indicates a reinstall of same product/release with
	# successful verification of previously installed media.
	#
	if [ "$SKIPINST" = "yes" ] ; then
		continue
	else
		promptWEBDIR
		promptWEBJAVADIR
		promptWEBCGIDIR
		promptWEBCGIPREFIX
	
		DOT="$TUXDIR"
	fi

	return 0
}

#
# ICEBERG's POST-INSTALLATION SCRIPT for its Core package.
#
postinstalliceberg()
{
	FILESUFFIX1="1"
	INSTALLMSGSENT="yes"

	#
	# SKIPINST = "yes" indicates a reinstall of same product/release with
	# successful verification of previously installed media.    
	# Send license reminder, then bypass all postinstall logic via return.
	#
	if [ "$SKIPINST" = "yes" ] ; then
		Prompt_User "\nIf your license file is accessible, you may install it now.\nInstall license file?"
		if [ $? = 0 ] ; then
			INSTTARGDIR=$TUXDIR
			export INSTTARGDIR
			sh $TUXDIR/bin/lic.sh
		else
			remindLicUtilMsg
		fi
		return 0
	fi

	createICEenvfile $TUXDIR
	newTUXLocale `find $TUXDIR/locale -type d -name C -print 2>/dev/null`

	if [ "$WSONLY" = "yes" ] ; then
		createSuminfofile $TUXDIR/.clientsuminfo $FILESUFFIX1
	else
		createSuminfofile $TUXDIR/.suminfo 
	fi

	mvWebGuiFiles
	copySysReg

	#
	# Permissions may be changed afer Web GUI File moves, since we use
	# variable path names for this component.
	# Note that:
	#  - Web GUI files may or may not be installed under TUXDIR;
	#  - files that may be modified by the adminstrator have perm 644;
	#  - tuxadm may or may not be in a directory with "bin" in the name
	#
	if [ "$WSONLY" = "yes" ] ; then
		changeBEAPerms $TUXDIR/.clientsuminfo
	else
		changeBEAPerms $TUXDIR/.suminfo
	fi

	createTUXwsvrIni
	createWebGuiIni
	createTUXlicFile
	setTlistenPwd
        #
        # Separate suminfo files exist for each class - server/client and
        # client-only.  Pass the appropriate one based on class selected.
        #
	echo "\nVerifying installation..." >&2
        if [ "$WSONLY" = "yes" ] ; then
                verifyInst $TUXDIR/.clientsuminfo
        else
                verifyInst $TUXDIR/.suminfo
        fi                                                                     
	ckInstStatus
	
        if [ "$WSONLY" != "yes" ] ; then
		Prompt_User "\nIf your license file is accessible, you may install it now.\nInstall license file?"
		if [ $? = 0 ] ; then
			INSTTARGDIR=$TUXDIR
			export INSTTARGDIR
			sh $TUXDIR/bin/lic.sh
		else
			remindLicUtilMsg
		fi
	fi

	return 0
}

#
# Used to determine the location of the JOLT client files.
#
promptJOLTDIR()
{


        echo "\nThe client software should be installed either on your web" >&2
	echo "server machine, or a machine easily accessible to your web" >&2
	echo "server machine, as the class files must be downloaded." >&2
	echo "\nJOLTDIR below refers to the directory in which your java" >&2
	echo "releted files are stored.  It is the directory which contains" >&2
	echo "the directory 'classes', not the classes directory itself!\n" >&2

	while true ; do
	
		ANSWER=`ckpath -a \
			-d "$TUXDIR/udataobj/jolt" \
			-p "JOLTDIR" \
			-h "Consult your JOLT documentation for an appropriate directory"` || doExit $?
	
		valDIR "$ANSWER"
		if [ "$?" -eq 0 ] ; then
			break
		fi
	done

	# Pick up dir, since it may be different from ANSWER
	JOLTDIR=$dir
	mkdir $JOLTDIR >/dev/null 2>&1
	echo "\nUsing $JOLTDIR as the JOLT client tree" >&2

	#
	# If installing client only, make DOT point to JOLTDIR. 
	# DOT is the directory into which the installation is done.
	# If we are installing both client and server, the installation
	# directory, DOT, should not be changed here. All files will be
	# installed into TUXDIR and the client files will have to be
	# moved.
	#
	if [ "${WSONLY}" = yes ]; then
		DOT=$JOLTDIR
	fi
}

#
# Routines taken from Jolt's initial installation script used during
# the pre and postinstallation of BEA Jolt.
#
uninstallJoltServer ( ) {
        if [ -x $TUXDIR/joltsrv/uninstall ]
        then
                cd $TUXDIR/joltsrv
                echo "Uninstalling server files..."
                ./uninstall
                echo "Server files uninstalled."
        fi
        exit
}

ask () {

        echo
        echo $1 "($2)"
        eval val=$`eval echo $2`
        echo
        echo The default reply is:  $val
        echo Enter the new value for $2, or press
        echo the ENTER key to accept the default
        echo
        read answer
        case $answer in
                '?')    echo "Enter the new value for $2 or press enter "
                        echo "to accept the current value"
                        ;;
                '')     echo $2=`eval echo $"$2"`
                        ;;
                *)      eval "$2=$answer"
                        ;;
        esac
}

askyn () {
        echo
        echo $1 '[yes/no]'
        while [ 1 ]
        do
                read answer
                case $answer in
                        [Yy]*)  eval "$2=1"
                                return 1
                                ;;
                        [Nn]*)  eval "$2=0"
                                return 0
                                ;;
                        *)      echo "you must answer yes or no"
                                ;;
                esac
        done
}

checkTuxVersion () {
        release="`grep release $TUXDIR/.serialnumber 2>/dev/null`"
        tryanyway=1
        case "$release" in
                *6.1*)          ;;
                *6.2*)          ;;

                *Release*)      echo "The directory you have specified for"
                                echo "TUXDIR is $TUXDIR, and appears to have"
                                echo "a version of TUXEDO other than 6.1 or 6.2"
                                echo "Please note that Jolt 1.0.1 is supported"
                                echo "only on TUXEDO 6.1 and 6.2.  "
                                askyn "Do you want to try anyway?" tryanyway
                                ;;

                *)              echo "The directory you have specified for"
                                echo "TUXDIR is $TUXDIR, and appears to "
                                echo "not be a TUXEDO directory.  Installing"
                                echo "Jolt into a directory other than a"
                                echo "TUXEDO 6.1 or 6.2 directory is done at"
                                echo "your own risk, and should not be attempted
"
                                echo "unless supervised by BEA personnel."
                                echo " "
                                askyn "Do you want to try anyway?" tryanyway
                                ;;
        esac

        if [ $tryanyway = 0 ]; then
                eval "$1=0"
        else
                eval "$1=1"
        fi
}

promptLicense()
{
        pat='[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]'
        SERIAL=`ckstr -r "$pat" -l 10 \
                -h "The 10-digit serial number appears on the license informatio
n card" \
                -e "Invalid serial number; must have 10 digits" \
                -p "Jolt serial number"` || doExit $?

        x='[0-9A-Fa-f]'
#if [ "$MNEMONIC" = sun4x ] ; then
# expr can't handle long regex
#pat="$x$x*"
#else
        pat="$x$x$x$x$x$x$x$x$x$x$x$x$x$x$x$x"
#fi
        TOKEN=`ckstr -r "$pat" -l 16 \
                -h "The 16-character license token appears on the license information sheet" \
                -e "Invalid license token; must have 16 hexadecimal characters" \
                -p "Jolt license token"` || doExit $?
}

#
# valLicense: validate the license token, given SERIAL, TOKEN, TUXDIR, JOLTTYPE.
# Reprompt if necessary.
#
valLicense()
{

        if [ -z "$SERIAL" -o -z "$TOKEN" -o -z "$TUXDIR" -o -z "$JOLTTYPE" ]; then
                echo "ERROR: Internal error" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 3
        fi

        if [ ! -x "$JOUTIL" ] ; then
                echo "ERROR: Internal error (no joutil)" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 3
        fi

        while true ; do                                                        

                $JOUTIL -V -l "$TOKEN" -s "$SERIAL" -T "$JOLTTYPE" 2>/dev/null

                if [ "$?" -eq 0 ] ; then
                        break
                fi
                echo "ERROR: The license token <$TOKEN> and/or serial number <$SERIAL>" >&2
                echo "is invalid.  Please enter these again" >&2
                promptLicense
        done
}

#
# activateLicense: validate and burn in the license in the installed binaries.
# JOUTIL, TUXDIR, TOKEN, SERIAL, JOLTTYPE must be set.
#
activateLicense ( )
{

        valLicense

        echo "Activating the license for JOLT software in $TUXDIR ..." >&2
        $JOUTIL -b -r "$TUXDIR" -l "$TOKEN" -s "$SERIAL" -T "$JOLTTYPE" > /dev/null
        if [ $? -ne 0 ] ; then
                echo "ERROR: Unable to activate the license for this installation" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 3
        fi
        echo "... finished" >&2
}

#
# The jolt client files go in JOLTDIR as opposed to TUXDIR.
mvJoltClientFiles()
{

	#
	# Make sure the client directory exists (it was installed) and
	# that the new location has been set (JOLTDIR).
	#
	if [ -d $TUXDIR/client -a x$JOLTDIR != x ]; then

		echo
		echo "Moving JOLT client files to their proper location..." >&2
		cd $TUXDIR/client

		#
		# Make JOLTDIR if it doesn't exist.
		#
		if [ ! -d `dirname $JOLTDIR` ]; then
			mkdir `dirname $JOLTDIR` ]
		fi

		if [ ! -d $JOLTDIR ]; then
			mkdir $JOLTDIR
		fi

		cp -f . $JOLTDIR
		cd $TUXDIR
		rm -rf $TUXDIR/client

		echo "... finished" >&2
	
	fi
}
	

promptTUXDIR_forJOLT()
{

	while true ; do

		ANSWER=`ckpath -a \
			-p "TUXDIR" \
			-h "Specify a directory for the TUXEDO base directory"` || doExit $?

		valDIR "$ANSWER"
		if [ "$?" -eq 0 ] ; then
			break
		fi
	done

	#
	# libgp.a is installed on every TUXEDO platform.
	#
	if [ ! -f "$ANSWER/lib/libgp.a" ]; then
		echo "\nERROR: $ANSWER doesn't contain a valid TUXEDO installation.\n" >&2
		doExit 1
	fi

	# Pick up dir, since it may be different from ANSWER
	TUXDIR=$dir
	export TUXDIR
	DOT=$dir
	echo "\nUsing $TUXDIR as the TUXEDO base directory" >&2
}

promptJOLTOpts()
{

	ANSWER=`ckitem -l "The following installation options are available:" \
		-p "Select an option" \
		-d "both" \
		"both	Install the full JOLT System - client and server" \
		"client	Install the client only"` || doExit $?

	# Make sure correct CLASSES are known to checkSPACE
	case "$ANSWER" in
	both)	CLASSES='wsc'
		;;
	client)	WSONLY=yes
		CLASSES='ws'
		;;
	*)	doExit 1
		;;
	esac

}

#
# BEA Jolt's pre-installtion routine.
#
requestjolt()
{

	WSONLY=
	TUXDIR=
	DOT=
	
	#
	# This function will set the WSONLY and CLASSES 
	# variables. WSONLY means install the client only,
	# CLASSES is used to determine which of the cpio
	# files containing JOLT to install.
	#
	promptJOLTOpts

	#
	# If client only is selected, no reason to show this message.
	#
	if [ "$WSONLY" != yes ]; then
		trap "uninstallJoltServer" 1 2 3 15
        	echo "\nNote that the jolt server will be installed into an existing" >&2
        	echo "TUXEDO directory.  You MUST have previously installed TUXEDO" >&2
        	echo "version 6.1 or 6.2 to attempt this installation." >&2
        	echo >&2

		promptTUXDIR_forJOLT
	fi

	#
	# Jolt server files go into an existing TUXDIR, the client 
	# files may go into JOLTDIR. If the customer is 
	# installing client only, do not prompt for TUXDIR. If the 
	# customer is installing both client and server, prompt for
	# both TUXDIR and JOLTDIR.
	#
	promptJOLTDIR
	return 0
}

postinstalljolt()
{

	#
	# Prompt for license information if installing
	# Jolt server.
	#
	if [ "$WSONLY" != yes ]; then
		promptLicense
        	JOUTIL=$TUXDIR/bin/joutil
		echo "JOUTIL is $JOUTIL"
        	JOLTTYPE=SDK
        	activateLicense
	fi

	#
	# If this is a client only installation, the suminfo
	# file should be placed in JOLTDIR as opposed to TUXDIR,
	# since we don't know TUXDIRs location.
	#
	if [ "${WSONLY}" != yes ]; then
		createSuminfofile $TUXDIR/.joltsuminfo
		changeTUXPerms $TUXDIR/.joltsuminfo
		mvJoltClientFiles
	else
		createSuminfofile $JOLTDIR/.joltsuminfo
		changeTUXPerms $JOLTDIR/.joltsuminfo
	fi

	return 0
}

#
# BEA Connect's PRE-INSTALLATION SCRIPT for its TPS package.
#
requesttps()
{

	DOT=

	#
	# The classes variable must be set by the request
	# script so that the file that contains the package
	# is known.
	#
	CLASSES='tps'

	promptTPSDIR
	DOT="$TPSDIR"
	return 0
}

#
# BEA Connect's POST-INSTALLATION SCRIPT for its TPS package.
#
postinstalltps()
{

	createSuminfofile $TPSDIR/.tpssuminfo
	#
	# Can use TUXEDO's permissions function to make
	# sure the permissions are what we want on the files
	# installed.
	#
	changeTUXPerms $TPSDIR/.tpssuminfo
	return 0
}

#
# BEA X-GUI's PRE-INSTALLATION SCRIPT.
#
requestxgui()
{

	DOT=

	#
	# The classes variable must be set by the request
	# script so that the file that contains the package
	# is known.
	#
	CLASSES='xgui'

	promptXGUIDIR
	DOT="$XGUIDIR"
	return 0
}

#
# BEA XGUI's POST-INSTALLATION SCRIPT.
#
postinstallxgui()
{

	createSuminfofile $XGUIDIR/.xguisuminfo
	#
	# Make sure the permissions are what we want on the files
	# installed.
	#
	changeTUXPerms $XGUIDIR/.xguisuminfo
	return 0
}

#
# BEA TUXEDO's Encryption Package request script.
#
requestenc40()
{

	#
	# Need a temp file.
	# 
	ENCTMP=/tmp/BEA3.$$
	> $ENCTMP

	#
	# The classes variable must be set by the request
	# script so that the file that contains the package
	# is known.
	#
	CLASSES=$PKG
	ENCPKG=$PKG	#remember which package

	#
	# Having Millennia3 2.1 installed is a pre-req for the Enc40
	# package.
	#
	echo "\nMillennia3 2.1 is required to install the Security Add-On Package" >&2
	getTUXDIR

	#
	# DOT is the base directory into which the package gets installed.
	#
	DOT="$TUXDIR"

	#
	# IRIX 5.3 doesn't have a what command, so skip...
	#
	if [ `uname -s` != IRIX -a `uname -r` != 5.3 ]; then

		#
		# Should not install the encryption library if the
		# existing libgp's version doesn't match that of the 
		# new file.
		#
		CURLEVEL=`what ${TUXDIR}/lib/$GPFILENAME | grep Port \
			  | sed -e 's/^[ 	]*//'`
		#
		# PORT comes from the pkginfo file.
		#
		NEWLEVEL=`echo ${PORT}`
		if [ "${CURLEVEL}" != "${NEWLEVEL}" ]; then
			echo "ERROR: Must point to $NEWLEVEL to install."
			exit 1
		fi

		#
		# Determine the level of encryption already installed.
		#
		CURCRYPT=`what ${TUXDIR}/lib/$GPFILENAME | grep -i encry | grep [0-9]`
		echo "${CURCRYPT}" > $ENCTMP

		if [ `grep -c 128 $ENCTMP` = 1 ]
		then
			CURCRYPT=128
		elif [ `grep -c 40 $ENCTMP` = 1 ]
		then
			CURCRYPT=40
		elif [ `grep -c 0 $ENCTMP` = 1 ]
		then
			CURCRYPT=0
		else
			echo "\nERROR: Could not determine encryption level" >&2
			rm -rf $ENCTMP
			doExit 1
		fi
	else
		#
		# IRIX 5.3 - set CURCRYPT to IRIX53
		#
		CURCRYPT=IRIX53
	fi

	rm -rf $ENCTMP

	#
	# Move the current libgp to a backup in case there is a problem.
	#
	echo "\nMoving ${TUXDIR}/lib/$GPFILENAME to ${TUXDIR}/lib/$GPFILENAME.${CURCRYPT}" >&2

	mv ${TUXDIR}/lib/$GPFILENAME ${TUXDIR}/lib/$GPFILENAME.${CURCRYPT}
	
	if [ "$?" -ne 0 ]; then
		echo "\nERROR: Cannot backup $GPFILENAME...an application may be running" >&2
		doExit 1
	fi

	#
	# If we are not running on AIX, we can move the current libgp.a
	# to a backup in case user reverts back to a previous encryption level.
	#

	if [ "$GPFILENAME" != "$GPFILENAMEA" ] ; then

		echo "\nMoving ${TUXDIR}/lib/$GPFILENAMEA to ${TUXDIR}/lib/$GPFILENAMEA.${CURCRYPT}" >&2

		mv ${TUXDIR}/lib/$GPFILENAMEA ${TUXDIR}/lib/$GPFILENAMEA.${CURCRYPT}
	
		if [ "$?" -ne 0 ]; then
			echo "\nERROR: Cannot backup $GPFILENAMEA...an application may be running" >&2
			doExit 1
		fi

	fi

	return 0
}

requestenc128()
{
	requestenc40
}

#
# BEA TUXEDO's Encryption post-installation script.
#
postinstallenc40()
{

	if [ "$ENCPKG" = enc40 ]; then
		createSuminfofile $TUXDIR/.enc40suminfo
	else
		createSuminfofile $TUXDIR/.enc128suminfo
	fi

	#
	# Make sure the permissions on the new libgp 
	# are correct.
	#
	changeENCPerms

	return 0
}

postinstallenc128()
{
	postinstallenc40
}

requesttuxdoc()
{

	DOCDIR=
	DOT=

	promptDOCOpts

	# WARNING: TUXDIR must be set
	promptDOCDIR $TUXDIR

	return 0

}

postinstalltuxdoc()
{

	#
	# If this platform's CD driver forced the copy
	# to take place in upper case, we need to rename
	# the files so that Tuxedo links will find them.
	#
	if [ "${uppercase}" = TRUE ]; then

		cwd=`pwd`
		cd $DOCDIR	

                echo "Renaming doc files..." >&2
		#
		# Do this in two steps, first the files...
		#
		find DOC -type f -print | while read line
		do 
			filename=`basename $line`
			d=`dirname $line`
			new_filename=`echo $filename | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz' ` 
			mv $d/$filename $d/$new_filename 2>/dev/null
		done

		#
		# Now the directories...
		#
		find DOC -type d -depth -print | while read line
		do
			dir=`basename $line`
			d=`dirname $line`
			new_dirname=`echo $dir | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz'` 
			mv $d/$dir $d/$new_dirname
		done

	#
	# Go back to the directory in which we started.
	#
	cd $cwd	
	fi

	createSuminfofile $DOCDIR/.tuxdocsuminfo
	#
	# Make sure the permissions are what we want on the files
	# installed.
	#
	changeTUXPerms $DOCDIR/.tuxdocsuminfo

	return 0
}

requestmsgqdoc()
{

	DOCDIR=
	DOT=

	CLASSES='msgqdoc'

	promptDOCDIR $BEADIR

	return 0

}

postinstallmsgqdoc()
{

	#
	# If this platform's CD driver forced the copy
	# to take place in upper case, we need to rename
	# the files so that Tuxedo links will find them.
	#
	if [ "${uppercase}" = TRUE ]; then

		cwd=`pwd`
		cd $DOCDIR	

                echo "Renaming doc files..." >&2
		#
		# Do this in two steps, first the files...
		#
		find DOC -type f -print | while read line
		do 
			filename=`basename $line`
			d=`dirname $line`
			new_filename=`echo $filename | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz' ` 
			mv $d/$filename $d/$new_filename 2>/dev/null
		done

		#
		# Now the directories...
		#
		find DOC -type d -depth -print | while read line
		do
			dir=`basename $line`
			d=`dirname $line`
			new_dirname=`echo $dir | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz'` 
			mv $d/$dir $d/$new_dirname
		done

	#
	# Go back to the directory in which we started.
	#
	cd $cwd	
	fi

	createSuminfofile $DOCDIR/.msgqdocsuminfo
	#
	# Make sure the permissions are what we want on the files
	# installed.
	#
	changeTUXPerms $DOCDIR/.msgqdocsuminfo

	return 0
}

requestmqscdoc()
{

	DOCDIR=
	DOT=

	CLASSES='mqscdoc'

	promptDOCDIR $BEADIR

	return 0

}

postinstallmqscdoc()
{

	#
	# If this platform's CD driver forced the copy
	# to take place in upper case, we need to rename
	# the files so that Tuxedo links will find them.
	#
	if [ "${uppercase}" = TRUE ]; then

		cwd=`pwd`
		cd $DOCDIR	

                echo "Renaming doc files..." >&2
		#
		# Do this in two steps, first the files...
		#
		find DOC -type f -print | while read line
		do 
			filename=`basename $line`
			d=`dirname $line`
			new_filename=`echo $filename | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz' ` 
			mv $d/$filename $d/$new_filename 2>/dev/null
		done

		#
		# Now the directories...
		#
		find DOC -type d -depth -print | while read line
		do
			dir=`basename $line`
			d=`dirname $line`
			new_dirname=`echo $dir | tr 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' 'abcdefghijklmnopqrstuvwxyz'` 
			mv $d/$dir $d/$new_dirname
		done

	#
	# Go back to the directory in which we started.
	#
	cd $cwd	
	fi

	createSuminfofile $DOCDIR/.mqscdocsuminfo
	#
	# Make sure the permissions are what we want on the files
	# installed.
	#
	changeTUXPerms $DOCDIR/.mqscdocsuminfo

	return 0
}

requestjoltdoc()
{

	JOLTDOCDIR=
	DOT=
	CLASSES=joltdoc

	# WARNING: TUXDIR must be set
	promptJOLTDOCDIR $TUXDIR

	return 0

}

postinstalljoltdoc()
{

	createSuminfofile $JOLTDOCDIR/.joltdocsuminfo
	#
	# Make sure the permissions are what we want on the files
	# installed.
	#
	changeTUXPerms $JOLTDOCDIR/.joltdocsuminfo

	return 0
}

#
# Change file permissions.
# This is a generic function for any package that installs under
# the BEA-standard directory structure.  This differs from the
# predecessor function changeTUXPerms, in the deletion of $PRODUCTDIR
# as a part of the chmod target.  The suminfo structure has changed
# to include a variable directory prefix based on the product and
# particular component, e.g, Web Gui, being installed.
# 
# Takes the sum information filename as its only parameter. (full pathname)
#
changeBEAPerms()
{
 
	#
	# Make sure the sum information file is passed as a parameter.
	#
	if [ -z "$1"  ]; then
		echo "ERROR changeTUXPerms: sum information file not passed" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 1 
	fi

	#
	# The sum information file must be readable.
	#
	SUMFILENAME=$1
	if [ ! -r "$SUMFILENAME"  ]; then
		echo "ERROR changeTUXPerms: The sum information file is not readable" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 1 
	fi

	echo "\nChanging file permissions..." >&2
 
	#
	# Make directories 755, files in bin
	# and .sl files (on hp) in lib 555, and all else 444.
	#
	for i in `grep '^[0-9]' < $SUMFILENAME | sed -e 's/.* //'`
	do
		case $i in
		*/bin/*|*.sl|*examples/s/s_client|*examples/s/s_server)
			cmd="chmod 555 "$i" 2>/dev/null"
			eval $cmd
			;;
		*)
			cmd="chmod 444 "$i" 2>/dev/null"
			eval $cmd
			;;
		esac
	done
 
	echo "... finished" >&2
}

#
# Change file permissions.
# This is a generic function for any package that installs under
# a TUXDIR-like directory structure
# 
# Takes the sum information filename as its only parameter. (full pathname)
#
changeTUXPerms()
{
 
	#
	# Make sure the sum information file is passed as a parameter.
	#
	if [ -z "$1"  ]; then
		echo "ERROR changeTUXPerms: sum information file not passed" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 1 
	fi

	#
	# The sum information file must be readable.
	#
	SUMFILENAME=$1
	if [ ! -r "$SUMFILENAME"  ]; then
		echo "ERROR changeTUXPerms: The sum information file is not readable" >&2
                echo "Please contact customer support for assistance" >&2
                doExit 1 
	fi

	#
	# The assumption is that all the files we are changing
	# permissions on exist under the directory in which the
	# sum information file is contained.
	#
	PRODUCTDIR=`dirname $1`

	echo "\nChanging file permissions..." >&2
 
	#
	# Make directories 755, files in bin
	# and .sl files (on hp) in lib 555, and all else 444.
	#
	for i in `grep '^[0-9]' < $SUMFILENAME | sed -e 's/.* //' -e 's/${BEADIR}\///'`
	do
		case $i in
		*/bin/*|*.sl|*examples/s/s_client|*examples/s/s_server)
			chmod 555 $PRODUCTDIR/$i 2>/dev/null
			;;
		*)
			chmod 444 $PRODUCTDIR/$i 2>/dev/null
			;;
		esac
	done
 
	echo "... finished" >&2
}

#
# Change file permissions for the encryption libraries.
#
changeENCPerms()
{
 
	echo "\nChanging file permissions..." >&2
 
	#
	# Change permissions on lib/libgp files.
	# .sl files (on hp) need to be set to 555, all else are 444.
	#
	for i in `ls $TUXDIR/lib/libgp*`
	do
		case $i in
		*.sl)
			chmod 555 $i 2>/dev/null
			;;
		*)
			chmod 444 $i 2>/dev/null
			;;
		esac
	done
 
	echo "... finished" >&2
}

#
# Move web GUI HTML documents, Java scripts, and CGI programs to the
# directories specified by the user.
#
mvWebGuiFiles()
{

        if [ "$WSONLY" != yes ] ; then

		if [ -f $TUXDIR/udataobj/webgui/webguitop.html ]
		then
			sed -e "s!http://HOST/CGI-BIN!$WEBCGIPREFIX!" \
			    -e "s!TUXDIRVALUE!$TUXDIR!" \
			    <$TUXDIR/udataobj/webgui/webguitop.html \
			    >$TUXDIR/udataobj/webgui/webguitop.tmp
			mv -f $TUXDIR/udataobj/webgui/webguitop.tmp \
			      $TUXDIR/udataobj/webgui/webguitop.html
		fi

		WEBMSG=n
		if [ -d $TUXDIR/udataobj/webgui -a x$WEBDIR != x -a x$WEBDIR != x$TUXDIR/udataobj/webgui ]
		then
			echo
			echo "Moving web gui files to their proper location..." >&2
			WEBMSG=y
			cd $TUXDIR/udataobj/webgui
			if [ ! -d `dirname $WEBDIR` ]
			then
				mkdir `dirname $WEBDIR` ]
			fi
			if [ ! -d $WEBDIR ]
			then
				mkdir $WEBDIR
			fi
			cp -rf webguitop.html webgui.html bea_ems.gif traffic.gif $WEBDIR
			cd $TUXDIR
			chmod 644 $WEBDIR/webguitop.html $WEBDIR/webgui.html
			chmod 644 $WEBDIR/bea_ems.gif $WEBDIR/traffic.gif
		fi
	
		if [ -d $TUXDIR/web/gui/java -a x$WEBJAVADIR != x -a x$WEBJAVADIR != x$TUXDIR/web/gui/java ]
		then
			if [ $WEBMSG = n ]
			then
				echo
				echo "Moving web gui files to their proper location..." >&2
				WEBMSG=y
			fi
			cd $TUXDIR/web/gui/java
			if [ ! -d $WEBJAVADIR ]
			then
				mkdir $WEBJAVADIR
			fi
			cp -Rf . $WEBJAVADIR
			cd $TUXDIR
			rm -rf $TUXDIR/web/gui/java
		fi
	
		if [ -d $TUXDIR/bin -a x$WEBCGIDIR != x -a x$WEBCGIDIR != x$TUXDIR/bin ]
		then
			if [ $WEBMSG = n ]
			then
				echo
				echo "Moving web gui files to their proper location..." >&2
				WEBMSG=y
			fi
			if [ ! -d $WEBCGIDIR ]
			then
				mkdir $WEBCGIDIR
			fi
			cp -f $TUXDIR/bin/tuxadm $WEBCGIDIR/tuxadm
			chmod 555 $WEBCGIDIR/tuxadm
		fi
	
		if [ $WEBMSG = y ]
		then
			echo "... finished" >&2
		fi
	
		# Remove original web directories if they are now empty.
		if [ -d $TUXDIR/web/gui -a "x`cd $TUXDIR/web/gui; echo *`" = 'x*' ]
		then
			rmdir $TUXDIR/web/gui
		fi
		if [ -d $TUXDIR/web -a "x`cd $TUXDIR/web; echo *`" = 'x*' ]
		then
			rmdir $TUXDIR/web
		fi
	fi
}

#
# Create System Registry for ORB from appropriate template, based on
# type of install (Both or Client-Only).  Copy, rather than move file
# so subsequent install verifications won't fail.
#
copySysReg()
{
        if [ "$WSONLY" = yes ] ; then
		if [ -f $TUXDIR/udataobj/SysRegiiop.rdp ]
		then
			cp -f $TUXDIR/udataobj/SysRegiiop.rdp \
			      $TUXDIR/udataobj/System.rdp 2>/dev/null 
		fi
	else
		if [ -f $TUXDIR/udataobj/SysRegtgiop.rdp ]
		then
			cp -f $TUXDIR/udataobj/SysRegtgiop.rdp \
			      $TUXDIR/udataobj/System.rdp 2>/dev/null 
		fi
	fi
}

#
# This function is used to determine if the driver for the
# CD displays the filenames in uppercase or lowercase. Set 
# the variable "uppercase" accordingly.
#
UpperCase()
{
	#
	# We know the file "SPACE" has to be in the
	# directory, which we are in now. Check to
	# see if the file is readable as a lower case 
	# filename first, if it's not check uppercase.
	# If neither is found there is a fatal ERROR, so
	# don't return.
	#
	if [ -r "space" ]; then
		uppercase=FALSE
	elif [ -r "SPACE" ]; then
		uppercase=TRUE
	else
		echo "ERROR: Documentation packaging problem." >&2
		echo "Please contact customer support for assistance." >&2
		doExit 3
	fi
	return 0
}

#
# This function determines whether or not the doc 
# directory structure is on the CD. If it is not return
# FALSE 1, else return TRUE 1.
#
DocOnCD()
{
	returndir=`pwd`
	cd $CDROOT
	if [ "${uppercase}" = TRUE ]; then
		if [ ! -d DOC ]; then
			cd .. 2>/dev/null
			if [ ! -d DOC ]; then
				#
				# DOC is not on CD, return FALSE.
				#
				cd $returndir
				return 1
			fi
			CDROOT=`pwd`
		fi
		#
		# DOC is there. Save off the name of the sub-directory, then return TRUE.
		#
		cd DOC
	else
		#
		# Files on CD are in lower case.
		#
		if [ ! -d doc ]; then
			cd ..
			if [ ! -d doc ]; then
				#
				# doc is not on CD, return FALSE.
				#
				cd $returndir
				return 1
			fi
			CDROOT=`pwd`
		fi
		#
		# doc is there. Save off the name of the sub-directory, then return TRUE.
		#
		cd doc
	fi
	docsubdir=`ls -d */*`
	cd $returndir
	return 0
}

#
# This function performs checksum on all files listed in $suminfofilename,
# comparing result to sum in $suminfofilename.  Failures are logged in
# install.log.
#
verifyInst()
{
	SUMEXEC=$TUXDIR/sumexec.sh
	INSTALLRESULT=
	SUMINFOFILENAME=$1

	#
	# Make sure there is a sum command on this
	# platform.
	#
	FOUND_SUM="`type sum | grep ' is '`"
 
	if [ -n "${FOUND_SUM}" ] ; then
		#
		# Create shell script to execute checksum on files contained
		# in $SUMINFOFILENAME.
		#
		awk 'BEGIN {count=0}

		{ if (count > 1) { 
			print "if [ ! -f "$3" ] ; then echo "$3" \"... file not found\" ;  fi 2>/dev/null" 
			print "if [ `sum "$3"|cut -d\" \" -f1` != "$1" ] ; then echo "$3" \"... checksum failed\" ;  fi 2>/dev/null" }
		} 

		{ if ($1 == "") count+=1 }

		' TUXDIR=$TUXDIR $SUMINFOFILENAME > $SUMEXEC  

		sh $SUMEXEC > $TUXDIR/install.log
		rm -f $SUMEXEC 2>/dev/null
        else
                echo "\nNo sum command available...\n" >&2
        fi
}

#
# This function generates success/failure message based on result of 
# checksum compare on all installed files performed in verifyInst().  
#
ckInstStatus()
{
	INSTALLRESULT=`wc -w $TUXDIR/install.log|awk '{print $1}'`
	if [ $INSTALLRESULT = 0 ] ; then
		echo "... Installation successful!"
		rm -f install.log 2>/dev/null
	else
		echo "Errors were encountered during install verification."
		echo "Check install.log in your install target directory for details."
	fi                                   	
}

#
# This function sets SKIPINST flag based on result of checksum compare on 
# all installed files performed in verifyInst(). If all files are present
# then we can skip most of the installation logic.
#
ckReinstStatus()
{
	INSTALLRESULT=`wc -w $TUXDIR/install.log|awk '{print $1}'`
	if [ $INSTALLRESULT = 0 ] ; then
		SKIPINST="yes"
	else
		SKIPINST="no"
	fi                                   	

	rm -f $TUXDIR/install.log 2>/dev/null
}


#
# MAIN PROCESSING
#
umask 022
trap "echo '\nInstallation terminated' >&2; exit 0" 2 15


a=`getplatforms | wc -l | sed -e 's/.*[ 	]//'`
if [ "$a" = 1 ]
then
	ANSWER=`getplatforms`
	PLATFORM=$ANSWER
	DisplayChoice
else
	PresentPlatforms
	GetPlatformNumber
	ConvertNumberToDirectory
	DisplayChoice
	#
	# Confirm the selection.
	#
	ANSWER=
	while [ -z "${ANSWER}" ]; do

		echo "Is this correct? [y,n,q]: \c" >&2
		read ANSWER

        	case "${ANSWER}" in
        	y|Y)
			#
			# Ok with selection. Continue with install.
			#
                	break
        	;;
		n|N)
			#
			# Not happy with selection - give the user
			# another chance.
			#
			PresentPlatforms
			GetPlatformNumber
			ConvertNumberToDirectory
			DisplayChoice
			ANSWER=
		;;
        	q|Q)
			doExit 0
        	;;
        	*) 
			# 
			# Not y, n or q. Ask again.
			#
			ANSWER=
        	;;         
		esac
	done
fi

#
# Set the environment for the installation process based
# on the MNEMONIC.
#
MNEMONIC=`echo ${PLATFORM} | awk -F/ '{print $1}'`

case "${MNEMONIC}" in
aix*|AIX*|dux|DUX|hpux11|HPUX11|sol*|SOL*)
	supportsM3=y
;;
*)
	supportsM3=n
;;
esac
export supportsM3

case "${MNEMONIC}" in
alpha|ALPHA|dux|DUX)
	envAlpha
;;
dgintl|DGINTL)
	envDgintl
;;
dgm88k|DGM88K)
	envDgm88k
;;
hp*|HP*)
	envHp
;;
ibm|IBM|aix*|AIX*)
	envIbm
;;
ncr|NCR|mpras|MPRAS)
	envNcr
;;
pyr|PYR)
	envPyr
;;
sco*|SCO*)
	envSco
;;
seq|SEQ|dynix|DYNIX)
	envSeq
;;
sgi|SGI|irix64|IRIX64)
	envSgi
;;
sni|SNI)
	envSni
;;
sun4x|SUN4X)
	envSun4x
;;
sun5x|SUN5X|sol*|SOL*)
	envSun5x
;;
tandem|TANDEM)
	envTandem
;;
unisys|UNISYS)
	envUnisys
;;
uw|UW)
	envUw
;;
*)
	envGeneric
;;
esac

#
# The root of the cd.
#
CDROOT=`pwd`

#
# Go to the installation directory of choice and
# execute the main processing loop.
#
cd $PLATFORM

#
# Remember where we are so we can get back.
#
STARTDIR=`pwd`

export PATH
#
# Tell how to quit
#
echo "\nTo terminate the installation at any time" >&2
echo "press the interrupt key," >&2
echo "typically <del>, <break>, or <ctrl+c>." >&2
WSONLY=
MAXPKGS=0
NUMPKGS=0
#
# Main processing
#
while [ true ] ; do
	promptPkgs || doExit $?
	MAXPKGS=`echo "$PKGS"|wc -w`
	installPkgs || doExit $?
	if [ -n "$ALLPKGS" ] ; then
		break
	fi
	NUMPKGS=`expr $NUMPKGS + 1 2>/dev/null`
	if [ $NUMPKGS = $MAXPKGS ] ; then
		break
	fi
done
finisMsg

exit 0
