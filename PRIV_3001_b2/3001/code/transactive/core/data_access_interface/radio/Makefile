# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2009/12/15 19:31:45 $
# Last modified by:  $Author: grace.koh $
#

PROJ_ROOT_DIR = ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/radio
PROJ_NAME = TA_RadioDAI_4677
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# The source files
#
PROJ_SOURCE = ConfigRadioDirectoryEntry.cpp \
		ConfigRadioDirectoryMember.cpp \
		ConfigRadioPredefinedTextMessage.cpp \
		ConfigRadioSubscriber.cpp \
		ConfigRadioTextMessageInbox.cpp \
		RadioBtnCfg.cpp \
		RadioBtnCfgFactory.cpp \
		RadioBtnCfgHelper.cpp \
		RadioDirectoryEntry.cpp \
		RadioDirectoryEntryFactory.cpp \
		RadioDirectoryEntryHelper.cpp \
		RadioDirectoryMember.cpp \
		RadioDirectoryMemberFactory.cpp \
		RadioDirectoryMemberHelper.cpp \
		RadioPredefinedTextMessage.cpp \
		RadioPredefinedTextMessageFactory.cpp \
		RadioPredefinedTextMessageHelper.cpp \
		RadioSubscriber.cpp \
		RadioSubscriberFactory.cpp \
		RadioSubscriberHelper.cpp \
		RadioTextMessageInbox.cpp \
		RadioTextMessageInboxFactory.cpp \
		RadioTextMessageInboxHelper.cpp \


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
