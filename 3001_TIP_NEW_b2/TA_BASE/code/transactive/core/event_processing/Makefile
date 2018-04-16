# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event_processing/Makefile $ 
# Author:  Ripple
# Version: $Revision: #3 $
#
# Last modification: $DateTime: 2014/07/14 20:22:57 $
# Last modified by:  $Author: Noel $
#
# This file builds the event processing library
#

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/event_processing
PROJ_NAME	= TA_EventProcessing
PROJ_SOURCE	= AbstractCommand.cpp \
CommandScheduler.cpp \
EventLoggingProcessor.cpp \
EventProcessingLibrary.cpp \
ProcessCommand.cpp \
RetryCommand.cpp \
RealTimeEventQueue.cpp \
RealTimeEventSender.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
    	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/event/IDL
		
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
