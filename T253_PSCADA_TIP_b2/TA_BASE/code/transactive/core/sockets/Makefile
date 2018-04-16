# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/sockets/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2017/10/31 16:21:54 $
# Last modified by:  $Author: CM $
#
# This Makefile builds the Sockets library.
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/threads
PROJ_NAME	= TA_Sockets
PROJ_SOURCE	= TcpSocket.cpp \
			  Datagram.cpp \
			  ICMPMessageCodec.cpp \
			  InetAddress.cpp \
			  RawDatagramSocket.cpp \
			  AbstractSocketOption.cpp \
			  SocketOptions.cpp \
			  TcpNoDelay.cpp \
			  ObservedSocket.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
