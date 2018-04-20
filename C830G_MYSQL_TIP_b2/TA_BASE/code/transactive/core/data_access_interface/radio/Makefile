# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/radio/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/radio
PROJ_NAME	= TA_RadioDAI
PROJ_SOURCE	= \
		  RAGroupMembersAccessFactory.cpp \
		  RAPatchMembersAccessFactory.cpp \
		  RASpeedDialButton.cpp \
		  RASpeedDialButtonAccessFactory.cpp \
		  RASpeedDialSet.cpp \
		  RASpeedDialSetAccessFactory.cpp \
		  RASubscribers.cpp \
		  RASubscribersAccessFactory.cpp \
		  RAPredefinedSDSMessage.cpp \
		  RAPredefinedSDSMessageAccessFactory.cpp \
		  RACallStackHistory.cpp \
		  RACallStackHistoryAccessFactory.cpp \
		  ITSIGroup.cpp \
		  ConfigITSIGroup.cpp \
		  ITSIGroupAccessFactory.cpp \
		  RADAIHelper.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
