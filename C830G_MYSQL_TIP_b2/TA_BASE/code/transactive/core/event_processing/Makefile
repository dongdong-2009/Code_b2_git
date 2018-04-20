# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4669_T01271350/TA_BASE/transactive/core/event_processing/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
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
		  RetryCommand.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
    	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL
	
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
