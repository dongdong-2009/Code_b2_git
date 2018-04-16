/*	Copyright 1996 BEA Systems, Inc.	*/
/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF     	*/
/*	BEA Systems, Inc.                     	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) 1993 USL
	All rights reserved

	THIS IS UNPUBLISHED PROPRIETARY
	SOURCE CODE OF USL
	The copyright notice above does not
	evidence any actual or intended
	publication of such source code.
*/
#ifndef TMENV_H
#define TMENV_H 1

/* #ident	"@(#)gp:libgp/mach/sun_ev.h	60.8" */

#if defined(__STDC__) || defined(__cplusplus)
#define _TMPROTOTYPES 1
#endif

#ifdef i386
#define _TML_ENDIAN 1
#endif

#if defined(_TMPROTOTYPES)
#if !defined(_TMCONST)
#define _TMCONST	const
#endif
#else
#define _TMCONST
#endif

#ifdef _TMPROTOTYPES
#define _(a) a
#else
#define _(a) ()
#endif

#ifndef NOWHAT
static	char	h_tmenv[] = "@(#)sun_ev.h	42.1";
#endif

#define O_BINARY	0

/* This is only used in Windows NT for thread instance data */
#define	_TM_THREADVAR

#define	_TM_FAR
#define	_TM_NEAR
#define _TMDLLENTRY
#define _TM_CDECL

#include <sys/types.h>

/* figure out if this is sun/os 4.1 which is posix compliant */
#if defined(alloca)
#ifdef _alloca_h
#define sun41
#endif
#else
#include <alloca.h>
#ifdef _alloca_h
#define sun41
#else
#undef alloca
#endif
#endif

#if defined(_TMPROTOTYPES)
typedef size_t		isize_t;
typedef uid_t           iuid_t;
typedef gid_t           igid_t;
#else
typedef int             isize_t; /* used by mem*, str*, stdio functions */
typedef int             iuid_t;
typedef int             igid_t;
#ifndef sun41
typedef int		mode_t;
typedef int		pid_t;
#endif
#endif

#define _TMPAGESIZE 	512L

typedef	int		_TMXDRINT;
typedef	unsigned int	_TMXDRUINT;
typedef	long		TM32I;
typedef	unsigned long	TM32U;

#define _TMIGW
#define _TMIGWT
#define _TMIBUFT
#define _TMIDNW
#define _TMIFML
#define _TMIFML32
#define _TMIFS
#define _TMIGP
#define _TMINWI
#define _TMINWS
#define _TMIQM
#define _TMIRMS
#define _TMISQL
#define _TMITMIB
#define _TMITUX
#define _TMITUX2
#define _TMITUX2WSC
#define _TMITUXWSC
#define _TMIUSORT
#define _TMIWSC
#define _TMITRPC

#endif
