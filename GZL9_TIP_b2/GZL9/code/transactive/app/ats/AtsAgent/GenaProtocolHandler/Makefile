# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/FZL1/transactive/app/ats/AtsAgent/GenaProtocolHandler/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008-1-21 17:24:30 $
# Last modified by:  $Author: buildteam $
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/ats/AtsAgent/GenaProtocolHandler
PROJ_NAME	= TA_GenaProtocolHandler

PROJ_SOURCE	= \
				AtsGenaManager.cpp \
				GenaFactory.cpp \
				GenaProtocolManager.cpp \
				GenaRawTable.cpp \
				GenaBlkTableParser.cpp \
				GenaEsbTableParser.cpp \
				GenaFGTableParser.cpp \
				GenaPATableParser.cpp \
				GenaPISTableParser.cpp \
				GenaPlatformTableParser.cpp \
				GenaSwitchTableParser.cpp \
				GenaTrainTableParser.cpp \
				SignalStatusTableParser.cpp \
				PowerStatusDataStore.cpp \
				GenaPlatformDoorTableParser.cpp
	
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
		TA_AtsAgentCorbaDef \
		TA_CommonATS \
		TA_GenaLibrary

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/CommonATS \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL 
		
		

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

STD_INC_DIRS += $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Foundation/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Util/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/XML/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Net/include \

install:
	#Do Nothing

