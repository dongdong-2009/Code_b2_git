#!/bin/sh
#
#        File: makefile
#
# Description: Makefile to build all BEA MessageQ example programs.
#
CC=     cc
#
#
# Set debug option as required.
DEBUG=
#DEBUG=-g
#

# Uncomment PLAT_CFLAGS definition for your platform
PLAT_CFLAGS=
#PLAT_CFLAGS=-D_HPUX_SOURCE=1 -Aa			# HP-UX
#PLAT_CFLAGS=-Bdynamic					# SCO
CFLAGS= $(DEBUG) $(PLAT_CFLAGS) -I$(BEADIR)/include

# Uncomment PLAT_LIBS defintion for your platform
PLAT_LIBS=
#PLAT_LIBS=-lsocket		# SCO
#PLAT_LIBS=-lsocket -lnsl	# Client - UnixWare, Sequent, NCR, Solaris
#
# Use the appropriate runtime library.
LIBS=-L$(BEADIR)/lib -ldmq $(PLAT_LIBS)
#LIBS=-L$(BEADIR)/lib -ldmqcl $(PLAT_LIBS)		# Client library
#
BINARIES = x_attnam x_attnum x_atttmp x_avail x_basic x_bind x_exit \
                x_get x_getall x_getem x_getpri x_getsel x_getsho x_getw \
                x_locate x_putbig x_putslf  x_recovr  x_sbsder  x_sbsreg \
                x_sbsrez x_sbstat x_sdm x_select x_shopnd x_timer 

binaries: $(BINARIES)

#
x_attnam:	x_attnam.o
		$(CC) x_attnam.c $(CFLAGS) $(LIBS) -o x_attnam 
x_attnum:	x_attnum.o
		$(CC) x_attnum.c $(CFLAGS) $(LIBS) -o x_attnum
x_atttmp:	x_atttmp.o
		$(CC) x_atttmp.c $(CFLAGS) $(LIBS) -o x_atttmp
x_avail:	x_avail.o
		$(CC) x_avail.c $(CFLAGS) $(LIBS) -o x_avail
x_basic:	x_basic.o
		$(CC) x_basic.c  $(CFLAGS) $(LIBS) -o x_basic
x_bind:	        x_bind.o
		$(CC) x_bind.c  $(CFLAGS) $(LIBS) -o x_bind
x_exit:		x_exit.o
		$(CC) x_exit.c   $(CFLAGS) $(LIBS) -o x_exit 
x_get:		x_get.o
		$(CC) x_get.c $(CFLAGS) $(LIBS) -o x_get
x_getall:	x_getall.o
		$(CC) x_getall.c $(CFLAGS) $(LIBS) -o x_getall
x_getem:	x_getem.o
		$(CC) x_getem.c $(CFLAGS) $(LIBS) -o x_getem
x_getpri:	x_getpri.o
		$(CC) x_getpri.c $(CFLAGS) $(LIBS) -o x_getpri
x_getsel:	x_getsel.o
		$(CC) x_getsel.c $(CFLAGS) $(LIBS) -o x_getsel
x_getsho:	x_getsho.o
		$(CC) x_getsho.c $(CFLAGS) $(LIBS) -o x_getsho
x_getw:		x_getw.o
		$(CC) x_getw.c $(CFLAGS) $(LIBS) -o x_getw
x_locate:	x_locate.o
		$(CC) x_locate.c $(CFLAGS) $(LIBS) -o x_locate
x_putbig:	x_putbig.o
		$(CC) x_putbig.c $(CFLAGS) $(LIBS) -o x_putbig
x_putslf:	x_putslf.o
		$(CC) x_putslf.c $(CFLAGS) $(LIBS) -o x_putslf
x_recovr:	x_recovr.o
		$(CC) x_recovr.c $(CFLAGS) $(LIBS) -o x_recovr
x_sbsder:	x_sbsder.o
		$(CC) x_sbsder.c $(CFLAGS) $(LIBS) -o x_sbsder
x_sbsreg:	x_sbsreg.o
		$(CC) x_sbsreg.c $(CFLAGS) $(LIBS) -o x_sbsreg
x_sbsrez:	x_sbsrez.o
		$(CC) x_sbsrez.c $(CFLAGS) $(LIBS) -o x_sbsrez
x_sbstat:	x_sbstat.o
		$(CC) x_sbstat.c $(CFLAGS) $(LIBS) -o x_sbstat
x_sdm:		x_sdm.o
		$(CC) x_sdm.c $(CFLAGS) $(LIBS) -o x_sdm
x_select:	x_select.o
		$(CC) x_select.c $(CFLAGS) $(LIBS) -o x_select
x_shopnd:	x_shopnd.o
		$(CC) x_shopnd.c $(CFLAGS) $(LIBS) -o x_shopnd
x_timer:	x_timer.o
		$(CC) x_timer.c $(CFLAGS) $(LIBS) -o x_timer

clean::
	rm -f $(BINARIES)

clobber: clean
