# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/tvss_agent/TCPCommunication/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:12:57 $
# Last modified by:  $Author: grace.koh $

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/cctv/tvss_agent/TCPCommunication
PROJ_NAME	= TA_TCPCommunication
PROJ_SOURCE	= BaseTcpServer.cpp \
		ObservedSocket.cpp \
		TCPConnection.cpp \
		TcpConnectionException.cpp \
		TcpConnectionManager.cpp \
		TcpException.cpp \
		TcpMethodTimeoutException.cpp \
		TcpTransactionManager.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
