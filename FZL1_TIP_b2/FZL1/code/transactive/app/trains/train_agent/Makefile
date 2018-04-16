# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#
# Make file for the Train Agent 
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/trains/train_agent
PROJ_NAME	= TrainAgent

PROJ_SOURCE	= \
			AlarmUtility.cpp \
			AtsAgentInterface.cpp \
			AtsAgentObserver.cpp \
			AtsDataTypes.cpp \
			AtsMessageProcessor.cpp \
			AuditSender.cpp \
			CallEndWorkItem.cpp \
			CctvAgentInterface.cpp \
			CctvTasks.cpp \
			CctvZone.cpp \
			ChangeModeCommandObserver.cpp \
			DataCallEndWorkItem.cpp \
			DepotOperationModeManager.cpp \
			DepotTimsCctvManager.cpp \
			DriverCallTasks.cpp \
			DutyAgentInterface.cpp \
			DutyAgentObserver.cpp \
			MessageSender.cpp \
			OccOperationModeManager.cpp \
			OperationModeManager.cpp \
			PaTasks.cpp \
			RadioAgentInterface.cpp \
			RadioAgentObserver.cpp \
			RadioGroupCoordinator.cpp \
			RadioMessageProcessor.cpp \
			RadioSessionData.cpp \
			RightsChecker.cpp \
			StateUpdateManager.cpp \
			StationOperationModeManager.cpp \
			TimsCctvManager.cpp \
			TimsDriverCallManager.cpp \
			TimsModeManager.cpp \
			TimsModeTasks.cpp \
			TimsPaManager.cpp \
			TimsPecManager.cpp \
			TimsStatusManager.cpp \
			TimsTisManager.cpp \
			TisTasks.cpp \
			TrainActivityMonitor.cpp \
			TrainAgent.cpp \
			TrainAgentConfiguration.cpp \
			TrainAgentStatusManager.cpp \
			TrainAlarmState.cpp \
			TrainCallState.cpp \
			TrainCctvCorbaServant.cpp \
			TrainCctvState.cpp \
			TrainCommsState.cpp \
			TrainDriverCallCorbaServant.cpp \
			TrainInformation.cpp \
			TrainInformationCorbaServant.cpp \
			TrainOperationModeCorbaServant.cpp \
			TrainPaCorbaServant.cpp \
			TrainPaState.cpp \
			TrainPecCorbaServant.cpp \
			TrainPecState.cpp \
			TrainRadioGroupCorbaServant.cpp \
			TrainStateHelper.cpp \
			TrainStateUpdateCorbaServant.cpp \
			TrainTisCorbaServant.cpp \
			TrainTisState.cpp \
			TrainVoiceCallRequest.cpp \
			VersionGen.cpp \
			main.cpp \

PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
			TA_TrainRadioGroupiDai \
			TA_TrainStateDai \
			TA_TrainProtocolLibrary \
			TA_TrainTransactionModel \
			TA_TrainCorbaProxy \
			TA_TrainCommonLibrary \
			TA_TrainAgentObjectNames \
			TA_TrainAgentCorba \
			TA_AtsDai \
			TA_VideoSwitchAgentAccess \
			TA_VideoSwitchAgentCorbaDef \
			TA_ScadaCommonLib \
			TA_GenericAgent \
			TA_Bus_Event \
			TA_EventTriggeredPlans \
			TA_PlanAgentLibrary \
			TA_PlanAgentCorbaDef \
			TA_PlanDAI \
			TA_PlanAgentDataCorbaDef \
			TA_PlanManagerCorbaDef \
			TA_Alarm \
			TA_AlarmAgentCorbaDef \
			TA_EntityStatusUpdateCorbaDef \
			TA_ManagedProcess \
			TA_ProcessManagementCorbaDef \
			TA_DerivedDataPointDAI \
			TA_RadioAgentCorbaDef_4669 \
			TA_AtsAgentCorbaDef \
			TA_AuthenticationCorbaDef \
			TA_RightsLibrary \
			TA_AuthenticationLibrary \
		TA_ProcessManagementCorbaDef \
			TA_DutyMonitorFramework \
			TA_DutyAgentCorbaDef \
			TA_SecurityUtilities \
			TA_Naming \
			TA_Message \
			TA_EventProcessing \
			TA_Event \
			TA_EventAgentCorbaDef \
			TA_Timers \
			TA_Thread \
			TA_AuthenticationCorbaDef \
			TA_NamingCorbaDef \
			TA_MessageCorbaDef \
			TA_RightsAgentCorbaDef \
			TA_EntityAccess \
			TA_DAI \
			TA_AlarmRuleDAI \
			TA_DataAccess \
			TA_ConfigurationUpdates \
			TA_CorbaUtil \
			TA_CorbaDef \
			TA_Utilities \
			TA_Exceptions \
			TA_CommonExceptionsCorbaDef \
			TA_Uuid \
			TA_Version \
			TA_Synchronisation \
			TA_ocl \
			TA_mcl \
			clntsh \
			mysqlclient \
			ACE \
 
DEPENDENT_LIBRARIES	= $(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
	                  $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/ats \
	                  $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/TrainStateDai \
	                  $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/TrainRadioGroupiDai \
	                  $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy \
	                  $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainProtocolLibrary \
	                  $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainTransactionModel

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
