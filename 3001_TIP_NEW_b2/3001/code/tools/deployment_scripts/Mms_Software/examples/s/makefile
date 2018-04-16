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
BINARIES = s_client s_server

all:		$(BINARIES)

clean::
	rm -f $(BINARIES)

clobber: clean


s_client:	s_client.o s_getopt.o
		cc $(CFLAGS) s_client.o s_getopt.o $(LIBS) -o s_client

s_server:	s_server.o s_getopt.o
		cc $(CFLAGS) s_server.o s_getopt.o $(LIBS) -o s_server

