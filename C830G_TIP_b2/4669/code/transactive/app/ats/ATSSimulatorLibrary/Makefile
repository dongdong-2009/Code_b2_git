# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/ats/ATSSimulatorLibrary
PROJ_NAME	= TA_ATSSimulatorLibrary
PROJ_SOURCE	= ATSSimCommandMonitor.cpp \
		ATSSimCommsObserver.cpp \
		ATSSimModbusFrames.cpp \
		ATSSimTable.cpp \
		ATSSimTableAddressRegister.cpp \
		ATSSimTableAtsDSS.cpp \
		ATSSimTableAtsPlatform.cpp \
		ATSSimTableAtsTrain.cpp \
		ATSSimTableAtsWILD.cpp \
		ATSSimTableAtsWash.cpp \
		ATSSimTableAtsWatchdog.cpp \
		ATSSimTableEnum.cpp \
		ATSSimTableIscsAlarm.cpp \
		ATSSimTableIscsElectricalSubsection.cpp \
		ATSSimTableIscsPlatform.cpp \
		ATSSimTableIscsTrain.cpp \
		ATSSimTableIscsWash.cpp \
		ATSSimTableIscsWatchdog.cpp \
		ATSSimTableLogData.cpp \
		ATSSimTableLogSupport.cpp \
		ATSSimTableQueryProcessor.cpp \
		ATSSimUserProtocol.cpp \
		ATSSimUtility.cpp \
		ATSSimulator.cpp \
		ATSSimulatorCommandProcessor.cpp \
		ATSSimulatorModuleCreator.cpp


PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
