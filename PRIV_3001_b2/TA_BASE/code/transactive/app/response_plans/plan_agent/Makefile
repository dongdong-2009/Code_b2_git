# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2018/03/14 13:51:20 $
# Last modified by:  $Author: lim.cy $
#
# Make file for the Plan Agent
#

PROJECT_NUMBER = 4669

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/response_plans/plan_agent
PROJ_NAME	= PlanAgent
PROJ_SOURCE	= PlanAgentCorbaDefImpl.cpp \
			DatapointTriggerObserver.cpp \
			DssAbstractRule.cpp \
			DssDatapointRule.cpp \
			DssEngine.cpp \
			DssKnowledgeBase.cpp \
			DssRuleProcessor.cpp \
			DssManager.cpp \
			AAMessageSender.cpp \
			ActiveEndStep.cpp \
			ActiveAbortStep.cpp \
			ActiveDataPointCheckStep.cpp \
			ActiveDataPointSetStep.cpp \
			ActiveDecisionStep.cpp \
			ActiveJumpStep.cpp \
			ActiveAssignCameraToMonitorStep.cpp \
			ActiveAssignCameraToPresetStep.cpp \
			ActiveVmsBlankoutStep.cpp \
			ActiveVmsRestoreScheduleStep.cpp \
			ActiveVmsSetSceneStep.cpp \
			ActiveTrainPaStep.cpp \
			ActiveTrainTisStep.cpp \
			ActiveVideoWallStep.cpp \
			ActiveStationPaStep.cpp \
			ActiveStationTisStep.cpp \
			ActiveTerminatePlanStep.cpp \
			ActiveEvaluateExpressionStep.cpp\
			ActiveRunPlanStep.cpp \
			ActiveLaunchGuiAppStep.cpp \
			ActiveLaunchSchematicStep.cpp \
			ActiveActivePlanCheckStep.cpp \
			ActiveSendSmsMessageStep.cpp \
			ActiveFailureSummaryStep.cpp \
			PlanDetail.cpp \
			ActivePlan.cpp \
			ActivePlanBulletinBoard.cpp \
			ActivePlanCustomiseStepMessage.cpp \
			ActivePlanDecisionStepMessage.cpp \
			ActivePlanLogStepMessage.cpp \
			ActivePlanRunPlanStepMessage.cpp \
			ActivePlanLaunchGuiAppStepMessage.cpp \
			ActivePlanLaunchSchematicStepMessage.cpp \
			ActivePlanExecutingState.cpp \
			ActivePlanMessageQueue.cpp \
			ActivePlanMessageBlock.cpp \
			ActivePlanPauseMessage.cpp \
			ActivePlanPauseForOperatorMessage.cpp \
			ActivePlanDetailUpdateMessage.cpp \
			ActivePlanPromptStepMessage.cpp \
			ActivePlanFailureSummaryStepMessage.cpp \
			ActivePlanPausedState.cpp \
			ActivePlanDelayedState.cpp \
			ActivePlanOperatorPausedState.cpp \
			ActivePlanPendingState.cpp \
			ActivePlanReassignMessage.cpp \
			ActivePlanResumeMessage.cpp \
			ActivePlanState.cpp \
			ActivePlanStopMessage.cpp \
			ActivePlanCompleteMessage.cpp \
			ActivePlanUnassignMessage.cpp \
			ActivePromptStep.cpp \
			ActiveLogStep.cpp \
			ActiveStep.cpp \
			PlanAccessFactory.cpp \
			PlanAgent.cpp \
			PlanAgentUser.cpp \
			PlanExecutor.cpp \
			PlanExecutionEngine.cpp \
			RunPlanMethodObject.cpp \
			PlanHouseKeepingMethodObject.cpp \
			PlanNotFoundException.cpp \
			ActivePlanMessageException.cpp \
			ActiveStepParamException.cpp \
			ThirdPartyLibException.cpp \
			SortablePlanId.cpp \
			PlanDataWriter.cpp \
			GRCType3SubTaskMO.cpp \
			GRCType3SubTask.cpp \
			GRC3DataPointSetComplexStep.cpp \
			ActiveNullStep.cpp \
			ActivePlanCheckTimerStep.cpp \
			ActivePlanSetTimerStep.cpp \
			ActiveExecuteScriptStep.cpp \
			ActiveDataPointTimeCheckStep.cpp \
			ActivePlanPopupMftMessageStepMessage.cpp \
			ActivePopupMFTMessageStep.cpp \
			ActiveShutdownSystemStep.cpp \
			ActiveBackgroundLaunchSchematicStep.cpp \
			ActiveBackgroundLaunchGuiAppStep.cpp \
			ActiveBackgroundDecisionStep.cpp \
			ActiveBackgroundLogStep.cpp \
			ActiveBackgroundPromptStep.cpp \
			CMMessageSender.cpp \
			VersionGen.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS1	= \
            TA_ScadaProxyLibrary \
			TA_DataNodeLibrary \
			TA_DataPointLibrary \
			TA_ScadaCommonLib \
			TA_ScadaCommonCorbaDef \
			TA_DataRecording \
			TA_DataRecordingDAI \
			TA_Timers \
			TA_CorbaUtil \
			TA_Naming \
			TA_Message \
			TA_EventProcessing \
			TA_Event \
			TA_EventAgentCorbaDef \
			TA_Utilities \
			TA_Version \
			TA_RightsLibrary \
			TA_DutyAgentCorbaDef \
			TA_SecurityUtilities \
			TA_RightsAgentCorbaDef \
			TA_GenericAgent \
			TA_DerivedDataPointDAI \
			TA_Bus_Event \
			TA_AuthenticationLibrary \
			TA_AuthenticationCorbaDef \
			TA_CorbaDef \
			TA_NamingCorbaDef \
			TA_EventTriggeredPlans \
			TA_PlanAgentLibrary \
			TA_PlanAgentCorbaDef \
			TA_PlanDAI \
			TA_PlanAgentDataCorbaDef \
			TA_PlanManagerCorbaDef \
			TA_ManagedProcess \
			TA_ProcessManagementCorbaDef \
			TA_PlanAgentCorbaDef \
			TA_PlanAgentDataCorbaDef \
			TA_Alarm \
			TA_AlarmAgentCorbaDef \
			TA_EntityStatusUpdateCorbaDef \
			TA_DAI \
			TA_AlarmRuleDAI \
			TA_Synchronisation \
			TA_MessageCorbaDef \
			TA_Thread \

PROJ_LIBS_4669	= \
			TA_TISAgentDAI_4669 \
			TA_TISAgentAccess_4669 \
			TA_TISManagerCorbaDef_4669 \
			TA_TISAgentCorbaDef_4669 \
			TA_VideoSwitchAgentAccess \
			TA_VideoSwitchAgentCorbaDef\
			TA_MathEvaluation

PROJ_LIBS_4680	= TA_VmsAgentCorbaDef \
			TA_VideoSwitchAgentAccess \
			TA_VideoSwitchAgentCorbaDef \
			TA_VideoWallAgentCorbaDef \
			TA_VideoWallLayoutAgentDAI

PROJ_LIBS2	= TA_EntityAccess \
			TA_DAI \
			TA_DataAccess \
			TA_ConfigurationUpdates \
			TA_Exceptions \
			TA_CommonExceptionsCorbaDef \
			TA_DataPointLibrary \
			TA_DataPointAgentCorbaDef_Impl \
			TA_ScadaCommonLib \
			TA_ScadaCommonCorbaDef \
			TA_ScadaProxyLibrary \
			TA_PlanManagerCorbaDef \
			TA_PlanDataAccess \
			TA_Utilities \
			TA_Uuid \
 			TA_ocl \
			TA_mcl \
			ACE \
			clntsh \
			mysqlclient

ifeq ($(PROJECT_NUMBER), 4680)
	PROJ_LIBS = $(PROJ_LIBS1) $(PROJ_LIBS_4680) $(PROJ_LIBS2)
endif

ifeq ($(PROJECT_NUMBER), 4669)
	PROJ_LIBS = $(PROJ_LIBS1) $(PROJ_LIBS_4669) $(PROJ_LIBS2)
endif

             
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)

DEPENDENT_LIBRARIES_GENERAL = \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
			$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
			$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
			$(PROJ_ROOT_DIR)/code/transactive/core/threads \
			$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
			$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
			$(PROJ_ROOT_DIR)/code/transactive/core/corba \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
			$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming \
			$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/message \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/database \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent_4669 \
			$(PROJ_ROOT_DIR)/code/transactive/bus/signs_4669/tis_agent_access \
			$(PROJ_ROOT_DIR)/code/transactive/bus/signs_4669/TisManagerCorbaDef \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent_4669/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datapoint_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
			$(PROJ_ROOT_DIR)/code/transactive/core/process_management/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/PlanDataAccess \
			$(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/agent_access \
			$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
			$(PROJ_ROOT_DIR)/code/transactive/bus/mathematical_evaluation \
			$(PROJ_ROOT_DIR)/code/transactive/core/event \
			$(PROJ_ROOT_DIR)/code/transactive/core/timers \
			$(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
			$(PROJ_ROOT_DIR)/code/transactive/bus/event \
			$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
			$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
			$(PROJ_ROOT_DIR)/code/transactive/bus/history/data_recording \
			$(PROJ_ROOT_DIR)/code/transactive/core/ocl

DEPENDENT_LIBRARIES_4669	= \

DEPENDENT_LIBRARIES_4680	= $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/video_wall_layout_agent \
				  $(PROJ_ROOT_DIR)/code/transactive/app/cctv/video_wall_agent/IDL \
				  $(PROJ_ROOT_DIR)/code/transactive/app/signs/vms_agent/IDL

ifeq ($(PROJECT_NUMBER), 4669)
	DEPENDENT_LIBRARIES = $(DEPENDENT_LIBRARIES_GENERAL) $(DEPENDENT_LIBRARIES_4669)
endif

ifeq ($(PROJECT_NUMBER), 4680)
	DEPENDENT_LIBRARIES = $(DEPENDENT_LIBRARIES_GENERAL) $(DEPENDENT_LIBRARIES_4680)
endif

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
