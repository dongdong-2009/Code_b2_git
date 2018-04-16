# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #12 $
#
# Last modification: $DateTime: 2017/05/31 16:48:50 $
# Last modified by:  $Author: huang.wenbo $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/signs/tis_agent
PROJ_NAME	= TisAgent

PROJ_SOURCE	= Exceptions.cpp \
		AgentModeMonitor.cpp \
		MessageLibraryManager.cpp \
		MessageLibrarySynchroniser.cpp \
		MessageLibraryUpgrader.cpp \
		STISAdHocDisplayRequest.cpp \
		STISClearRequest.cpp \
		STISCommunicationsHandler.cpp \
		STISCurrentDisplayingMessageRequest.cpp \
		STISManager.cpp \
		STISMessage.cpp \
		STISPIDControlRequest.cpp \
		STISOCCStatusRequest.cpp \
		STISPredefinedDisplayRequest.cpp \
		STISStationStatusRequest.cpp \
		STISUpgradePredefinedMessageLibraryRequest.cpp \
		StatusMonitor.cpp \
		StringHelper.cpp \
		TISAgent.cpp \
		TTISManager.cpp \
		STISStatusRequestQueue.cpp \
		STISStatusProcessQueue.cpp \
		TISMessageDataDefinitionsHelper.cpp \
		VersionGen.cpp \
		DPRequestProcessor.cpp \
		TrainAgentSubscriber.cpp \
		ILocationStore.cpp \
		main.cpp \
        CCTOCCStatusRequest.cpp \
        MessageLibraryParser.cpp \
		StringConverter.cpp \
		LibrarySyncFailedManager.cpp \
		LibrarySyncFailedProcessor.cpp \
		WorkThread.cpp \
		DataPointCache.cpp \
		STISAllStationsStatusRequest.cpp \
		WorkThreadPool.cpp
		

PROJ_TYPE	= exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive $(PROJ_ROOT_DIR)/code/cots/curl/curl-7.21.7/include
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
		TA_CorbaUtil \
		TA_TISManagerCorbaDef_4669 \
		TA_TISAgentCorbaDef_4669 \
		TA_DataPointAgentCorbaDef_Impl \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_ScadaCommonLib \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_DataPointLibrary \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_MmsCorbaDef \
		TA_RightsLibrary \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_AuthenticationLibrary \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationCorbaDef \
		TA_Timers \
		TA_Thread \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDAI \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef \
		TA_Alarm \
		TA_Naming \
		TA_Message \
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_Uuid \
		TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
		TA_TISAgentDAI \
		TA_TISAgentAccess \
		TA_TISAgentDAI_4669 \
		TA_TISAgentAccess_4669 \
		TA_TrainSelectionComponent \
		TA_TrainCorbaProxy \
		TA_TrainCommonLibrary \
		TA_TrainAgentObjectNames \
		TA_TrainAgentCorba \
		TA_EntityAccess \
		TA_DAI \
                TA_AlarmRuleDAI \
		TA_DerivedDataPointDAI \
		TA_AtsAgentCorbaDef \
		TA_CorbaDef \
		TA_NamingCorbaDef \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_DataAccess \
		TA_ConfigurationUpdates \
		TA_Utilities \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_Version \
		TA_Sockets \
		TA_FTP \
		TA_SFTP \
		curl \
		TA_MathEvaluation \
		TA_Synchronisation \
		TA_mcl \
		TA_ocl \
		mysqlclient \
		clntsh \
		ACE \
		iconv

DEPENDENT_LIBRARIES = \
                $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent \
                $(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/bus/signs/tis_agent_access \
                $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainSelectionComponent

PROJ_CFLAGS = -DTA_ASSERT_LOG=1 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
