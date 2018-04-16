# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/FZL1/transactive/bus/ats/ats_agent/CommonATSData/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008-1-21 17:24:30 $
# Last modified by:  $Author: buildteam $
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/ats/ats_agent/CommonATS
PROJ_NAME	= TA_CommonATS

PROJ_SOURCE	= \
				AtsManager.cpp \
				DataCache.cpp \
				AtsBasData.cpp \
				AtsCctvData.cpp \
				AtsDssData.cpp \
				AtsEcsData.cpp \
				AtsPasData.cpp \
				AtsPidsData.cpp \
				AtsPsdData.cpp \
				AtsTisData.cpp \
				AtsTrainData.cpp \
				AtsTwpData.cpp \
				AtsWildData.cpp \
				CctvBooleanContext.cpp \
				CctvRealNumberContext.cpp \
				PIDSAgentInterface.cpp \
				PowerData.cpp \
				RawTable.cpp \
				SectorData.cpp \
				TrainCctvCamera.cpp \
				TrainDataLocation.cpp \
				WatchDogData.cpp \
				DataSynch.cpp \
				OccUpdateProcessor.cpp \
				PADataSynch.cpp \
				StationOccUpdateProcessor.cpp \
				PADispatcher.cpp \
				PIDSDispatcher.cpp \
				AtsDataNotificationManager.cpp \
				DataNotification.cpp \
				DataProcessorManager.cpp \
				PADataNotification.cpp \
				PIDSDataNotification.cpp \
				TrainDataNotification.cpp \
				AddressUtil.cpp \
				AgentOperationMode.cpp \
				AtsHelper.cpp \
				AtsTrainDataTimer.cpp \
				DataPointHelper.cpp \
				DataProcessorThread.cpp \
				PADataProcessor.cpp \
				PIDSDataProcessor.cpp \
				TrainDataProcessor.cpp \
				TrainPisData.cpp \
				TrainPISDataNotification.cpp \
				TrainPISDataProcessor.cpp 
				

						
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive \
		$(PROJ_ROOT_DIR)/sdk/include/transactive/core/message/IDL/src \
		$(PROJ_ROOT_DIR)/sdk/include/transactive/core/alarm/IDL/src \
		$(PROJ_ROOT_DIR)/sdk/include/transactive/core/exceptions/IDL/src \
		$(PROJ_ROOT_DIR)/sdk/include/transactive/bus/scada/common_library/IDL/src \
		$(PROJ_ROOT_DIR)/sdk/include/transactive 
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
		TA_AtsAgentCorbaDef \

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

