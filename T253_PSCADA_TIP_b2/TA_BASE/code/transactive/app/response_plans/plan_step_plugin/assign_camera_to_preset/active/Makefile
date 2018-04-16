PROJ_ROOT_DIR	= ../../../../../../..
PROJ_CODE_DIR	= transactive/app/response_plans/plan_step_plugin/assign_camera_to_preset/active
PROJ_NAME	= TA_PlanActiveAssignCameraToPresetStep
PROJ_SRC_DIR	= ./src
PROJ_SOURCE	= \
		ActiveAssignCameraToPresetStep.cpp \
		AssignCameraToPresetStepComponent.cpp \
		DllActiveInstance.cpp \
		VersionGen.cpp

PROJ_TYPE	= dlib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = TA_ActiveStep \
		TA_PlanStepCommon \
		TA_VideoSwitchAgentAccess \
		TA_VideoSwitchAgentCorbaDef \
		TA_CommonExceptionsCorbaDef \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_DataPointAgentCorbaDef_Impl \
		TA_MMSHelp \
		TA_CorbaUtil \
		TA_ScadaCommonLib \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_DataPointLibrary \
		TA_DerivedDataPointDAI \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_RightsLibrary \
		TA_SecurityUtilities \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationLibrary \
		TA_AuthenticationCorbaDef \
		TA_GenericAgent \
		TA_ConfigurationUpdates \
		TA_Naming \
		TA_NamingCorbaDef \
		TA_Message \
		TA_EventProcessing \
		TA_CorbaDef \
		TA_Alarm \
		TA_AlarmAgentCorbaDef \
		TA_EntityStatusUpdateCorbaDef \
		TA_MmsCorbaDef \
		TA_PlanCommonBus \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDataAccess \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_AlarmRuleDAI \
		TA_PlanAgentDataCorbaDef \
		TA_PlanDAI \
		TA_EntityAccess \
		TA_DAI \
		TA_Timers \
		TA_Thread \
		TA_MathEvaluation \
		TA_Utilities \
		TA_Version \
		TA_DataAccess \
		TA_Uuid \
		TA_Exceptions \
		TA_Synchronisation \
		TA_ocl \
		mysqlcppconn \
		clntsh \
		mysqlclient \
		ACE



DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/app/response_plans/plan_step_plugin/common \
 $(PROJ_ROOT_DIR)/code/transactive/app/response_plans/plan_step_plugin/common_active \
 $(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
 $(PROJ_ROOT_DIR)/code/transactive/core/versioning \
 $(PROJ_ROOT_DIR)/code/transactive/core/corba \
 $(PROJ_ROOT_DIR)/code/transactive/core/threads \
 $(PROJ_ROOT_DIR)/code/transactive/core/utilities \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
 $(PROJ_ROOT_DIR)/code/transactive/core/database \
 $(PROJ_ROOT_DIR)/code/transactive/core/message \
 $(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
 $(PROJ_ROOT_DIR)/code/transactive/core/uuid \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/process_management \
 $(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent/ \
 $(PROJ_ROOT_DIR)/code/transactive/core/naming/ \
 $(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/core/alarm/ \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/common \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/event_triggered \
 $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/PlanDataAccess \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/rights_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/duty_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/security/security_utilities \
 $(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsHelper \
 $(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/IDL \
 $(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/agent_access \
 $(PROJ_ROOT_DIR)/code/transactive/bus/cctv/video_switch_agent/agent_access \
 $(PROJ_ROOT_DIR)/code/transactive/bus//history/data_recording \
 $(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/data_recording_dai \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
 $(PROJ_ROOT_DIR)/code/transactive/core/timers \
 $(PROJ_ROOT_DIR)/code/transactive/core/ocl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/proxy_library \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataNodeAgentCorbaDef_Impl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/scada/DataPointAgentCorbaDef_Impl \
 $(PROJ_ROOT_DIR)/code/transactive/bus/mathematical_evaluation
				
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
