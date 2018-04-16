# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/Makefile $
# @author:  Ripple
# @version: $Revision: #5 $
#
# Last modification: $DateTime: 2014/07/14 20:22:57 $
# Last modified by:  $Author: Noel $
#
# Make file for the ECS Agent 
#

PROJ_ROOT_DIR = ../../../../..
PROJ_CODE_DIR = transactive/app/ecs/ECSAgent
PROJ_NAME     = ECSAgent
PROJ_SOURCE   =	\
			AbstractECSAgent.cpp \
			AutoModeMonitor.cpp \
			CalculatedExpression.cpp \
			CongestionProposalEngine.cpp \
			CtaEcsAgentCompleteState.cpp \
			CtaEcsAgentSummary.cpp \
			CtaEcsAlarms.cpp \
			CtaEcsAuditing.cpp \
			CtaEcsCachedConfig.cpp \
			CtaEcsCachedMaps.cpp \
			CtaEcsComms.cpp \
			CtaEcsDatapoints.cpp \
			CtaEcsSynchronisation.cpp \
			DataPointWriteQueueProcessor.cpp \
			EcsArrowManager.cpp \
			EcsHelpers.cpp \
			EcsZoneCongestionContext.cpp \
			EcsZoneDataSource.cpp \
			EmergencyNormalisationMonitor.cpp \
			EmergencyProposalEngine.cpp \
			IscsAliveWorker.cpp \
			main.cpp \
			MasterECSAgent.cpp \
			MasterModeExecutionWorker.cpp \
			MasterModeLocksManager.cpp \
			MasterModeDataPointWorkQueue.cpp \
			MasterModeManager.cpp \
			StationECSAgent.cpp \
			StationModeExecutionWorker.cpp \
			StationModeManager.cpp \
			VersionGen.cpp 

PROJ_TYPE     = exe
PROJ_INC_DIR  = ./src $(PROJ_ROOT_DIR)/code/transactive \
		 	$(PROJ_ROOT_DIR)/sdk/include/transactive

#
# Order is important here.  Specify with most dependent libraries at the bottom
#
PROJ_LIBS =	\
			TA_EcsAgentCorbaDef \
			TA_AtsAgentCorbaDef \
			TA_AuthenticationLibrary \
			TA_AuthenticationCorbaDef \
			TA_ScadaProxyLibrary \
			TA_DataPointAgentCorbaDef_Impl \
			TA_DataNodeAgentCorbaDef_Impl \
			TA_DataPointLibrary \
			TA_DataNodeLibrary \
			TA_ScadaCommonLib \
			TA_ScadaCommonCorbaDef \
			TA_DataRecording \
			TA_DataRecordingDAI \
			TA_MmsCorbaDef \
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
			TA_Message \
			TA_MessageCorbaDef \
			TA_EventProcessing \
			TA_Event \
			TA_EventAgentCorbaDef \
			TA_CorbaUtil \
			TA_Utilities \
			TA_Version \
			TA_Timers \
			TA_Thread \
			TA_Naming \
			TA_NamingCorbaDef \
			TA_Synchronisation \
			TA_ManagedProcess \
			TA_ConfigurationUpdates \
			TA_Utilities \
			TA_MathEvaluation \
			TA_DerivedDataPointDAI \
			TA_RightsLibrary \
			TA_AuthenticationLibrary \
			TA_DutyAgentCorbaDef \
			TA_SecurityUtilities \
			TA_ProcessManagementCorbaDef \
			TA_AuthenticationCorbaDef \
			TA_RightsAgentCorbaDef \
			TA_AuthenticationCorbaDef \
			TA_EntityAccess \
			TA_ECSDAI \
			TA_DAI \
			TA_AlarmRuleDAI \
			TA_DataAccess \
			TA_Exceptions \
			TA_CommonExceptionsCorbaDef \
			TA_CorbaDef \
			TA_Uuid \
			TA_ocl \
			TA_mcl \
			clntsh \
			mysqlclient \
			ACE

DEPENDENT_LIBRARIES = \
			$(PROJ_ROOT_DIR)/code/transactive/bus/ecs/ecs_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/ecs \

 
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)/code/transactive


PROJ_OUT_DIR = \
		$(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
 
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
