# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2010/06/25 12:09:17 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/signs/tis_agent
PROJ_NAME	= TisAgent

PROJ_SOURCE	= Exceptions.cpp \
		AgentModeMonitor.cpp \
		MessageLibraryManager.cpp \
		MessageLibrarySynchroniser.cpp \
		MessageLibraryUpgrader.cpp \
		STISAdHocDisplayRequest.cpp \
		STISApprovedRATISDisplayRequest.cpp \
		STISClearRequest.cpp \
		STISCommunicationsHandler.cpp \
		STISConfigurationDataRequest.cpp \
		STISCurrentDisplayingMessageRequest.cpp \
		STISManager.cpp \
		STISMessage.cpp \
		STISPIDControlRequest.cpp \
		STISOCCStatusRequest.cpp \
		STISPredefinedDisplayRequest.cpp \
		STISStationStatusRequest.cpp \
		STISTrainInformationMessage.cpp \
		STISUpgradePredefinedMessageLibraryRequest.cpp \
		StatusMonitor.cpp \
		StringHelper.cpp \
		TISAgent.cpp \
		TTISAdHocDisplayRequest.cpp \
		TTISApprovedRATISDisplayRequest.cpp \
		TTISClearRequest.cpp \
		TTISLibraryDownloadRequest.cpp \
		TTISLibraryUpgradeRequest.cpp \
		TTISManager.cpp \
		TTISMessage.cpp \
		TTISPredefinedDisplayRequest.cpp \
		TTISTimeScheduleDownloadRequest.cpp \
		TTISTimeScheduleUpgradeRequest.cpp \
		RATISMessenger.cpp \
		STISStatusRequestQueue.cpp \
		TISMessageDataDefinitionsHelper.cpp \
		VersionGen.cpp \
		DPRequestProcessor.cpp \
		TrainAgentSubscriber.cpp \
		TTISApprovedRATISClearRequest.cpp \
		ILocationStore.cpp \
		main.cpp

PROJ_TYPE	= exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
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
		TA_AuthenticationLibrary \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_Timers \
		TA_Thread \
		TA_GenericAgent \
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
		TA_MathEvaluation \
		TA_Synchronisation \
		clntsh \
		nnz10 \
		TA_ocl \
		ACE

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
