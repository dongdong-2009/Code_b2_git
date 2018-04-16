# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/utilities/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2018/01/11 16:23:18 $
# Last modified by:  $Author: hoa.le $
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/utilities
PROJ_NAME	= TA_Utilities
PROJ_TYPE	= lib
PROJ_SOURCE	= AlarmClock.cpp \
			CodeConverter.cpp \
			DataConversion.cpp \
			DateFormat.cpp \
			DebugSETranslator.cpp \
			DebugUtil.cpp \
			DebugUtilInit.cpp \
			DllHandler.cpp \
			FunctionEntry.cpp \
			GenericLogType.cpp \
			Hostname.cpp \
			ILogType.cpp \
			Logger.cpp \
			MatrixException.cpp \
			PreformattedLogType.cpp \
			RunParams.cpp \
			StdPubUtility.cpp \
			TAAssert.cpp \
			TA_String.cpp \
			ThreasdafeScreenOutput.cpp \
			Timer.cpp
		  
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing
