# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/FZL1/transactive/app/ats/AtsAgent/ModBusProtocolHandler/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008-1-21 17:24:30 $
# Last modified by:  $Author: buildteam $
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/ats/AtsAgent/ModBusProtocolHandler
PROJ_NAME	= TA_ModBusProtocolHandler

PROJ_SOURCE	= \
				AtsModBusManager.cpp \
				ModBusFactory.cpp \
				ModBusProtocolManager.cpp \
				BaseTableParser.cpp \
				PATableParser.cpp \
				PIDSTableParser.cpp \
				TrainTableParser.cpp \
				TrainPISTableParser.cpp \
				SIGStatusWorker.cpp \
				SIGRTUManager.cpp
	
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
		TA_AtsAgentCorbaDef \
		TA_CommonATS

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/CommonATS \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL 
		
		

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

