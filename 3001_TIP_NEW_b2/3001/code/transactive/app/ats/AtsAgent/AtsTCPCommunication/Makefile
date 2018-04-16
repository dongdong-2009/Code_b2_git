# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/3001/transactive/app/ats/AtsAgent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:12:57 $
# Last modified by:  $Author: grace.koh $
#
# Makefile for AtsTCPCommunication classes.
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/ats/AtsAgent/AtsTCPCommunication
PROJ_NAME	= TA_AtsTCPCommunication
PROJ_SOURCE	= AtsTcpServer.cpp \
		ObservedSocket.cpp \
 		TcpConnection.cpp \
		TcpConnectionException.cpp \
		TcpConnectionManager.cpp \
		TcpException.cpp \
		TcpMethodTimeoutException.cpp \
		TcpTransactionManager.cpp \
		AtsBackgroundProcessor.cpp \
		AtsTcpServerWrapper.cpp \
		
		

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

