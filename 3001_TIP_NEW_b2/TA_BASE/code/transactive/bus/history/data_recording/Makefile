# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/history/data_recording/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $
#
# Make file for the DataRecording classes.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/history/data_recording
PROJ_NAME	= TA_DataRecording
PROJ_SOURCE	= DataRecorder.cpp DataCache.cpp DbDataWriter.cpp ExceptionOnWriteErrorPolicy.cpp AlarmOnWriteErrorPolicy.cpp Datum.cpp DataRecorderDbDataAccess.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

