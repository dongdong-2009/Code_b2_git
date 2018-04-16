# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #10 $
#
# Last modification: $DateTime: 2013/02/01 14:58:08 $
# Last modified by:  $Author: huang.wenbo $
#
# Makefile for the Video Switch Agent
#

PROJ_ROOT_DIR 	= ../../../../..
PROJ_CODE_DIR	= transactive/app/cctv/video_switch_agent
PROJ_NAME 		= VideoSwitchAgent
PROJ_SOURCE 	= AddressHelper.cpp \
              	AgentModeMonitor.cpp \
              	Camera.cpp \
              	CommunicationsHandler.cpp \
              	EntityCreationFactory.cpp \
              	Exceptions.cpp \
              	FocusIrisControl.cpp \
              	main.cpp \
              	MovementControl.cpp \
              	StateUpdateBroadcaster.cpp \
              	StateUpdateSubscriber.cpp \
              	SuperimposedTextControl.cpp \
              	SwitchManager.cpp \
              	UserPrivilegeVerifier.cpp \
              	VersionGen.cpp \
              	VideoInput.cpp \
              	VideoMonitor.cpp \
              	VideoOutput.cpp \
              	VideoOutputGroup.cpp \
              	VideoSwitchResource.cpp \
              	VideoSwitchAgent.cpp \
	      	VisualAuditMessageSender.cpp \
              	WashWipeControl.cpp \
              	Quad.cpp \
              	RecordingUnit.cpp \
              	ConfigUpdateBroadcaster.cpp \
              	Sequence.cpp \
              	PDInterfaceFactory.cpp \
              	LocalLocation.cpp

PROJ_TYPE   	= exe
PROJ_INC_DIR 	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive

PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIB_DIRS	= ./protocols/imos_sdk_sparc/lib
			
PROJ_LIBS 		= TA_IMOSSwitchAgent \
			TA_MODBUSSwitchAgent \
			TA_VideoSwitchAgentCorbaDef_gzl9 \
			TA_VideoSwitchAgentAccess_gzl9 \
			TA_VideoSwitchAgentDAI \
			TA_TrainAgentObjectNames \
			TA_TrainAgentCorba \
			TA_AtsDai \
			TA_CorbaUtil \
			TA_GenericAgent \
			TA_Bus_Event \
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
			TA_EventTriggeredPlans \
			TA_PlanDAI \
			TA_DerivedDataPointDAI \
			TA_DataRecording \
			TA_DataRecordingDAI \
			TA_MmsCorbaDef \
			TA_Alarm \
			TA_RightsLibrary \
			TA_SecurityUtilities \
			TA_AuthenticationLibrary \
			TA_AuthenticationCorbaDef \
			TA_RightsAgentCorbaDef \
			TA_Naming \
			TA_AlarmAgentCorbaDef \
                        TA_EntityStatusUpdateCorbaDef \
			TA_Message \
                        TA_EventProcessing \
			TA_Event \
			TA_EventAgentCorbaDef \
			TA_PlanAgentLibrary \
			TA_PlanAgentCorbaDef \
			TA_PlanManagerCorbaDef \
			TA_PlanAgentDataCorbaDef \
			TA_CorbaDef \
			TA_NamingCorbaDef \
			TA_MessageCorbaDef \
			TA_ManagedProcess \
			TA_ProcessManagementCorbaDef \
			TA_DutyAgentCorbaDef \
			TA_EntityAccess \
			TA_DAI \
                        TA_AlarmRuleDAI \
			TA_DataAccess \
			TA_ConfigurationUpdates \
			TA_Timers \
			TA_Thread \
			TA_Uuid \
			TA_Utilities \
			TA_Version \
			TA_ModbusHelperLib \
			TA_ModbusComms \
			TA_Exceptions \
			TA_CommonExceptionsCorbaDef \
			TA_Sockets \
			TA_MathEvaluation \
			TA_Synchronisation \
			TA_mcl \
			TA_ocl \
			ACE \
			sdk \
			clntsh


DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/app/cctv/video_switch_agent/protocols/IMOS \
				      $(PROJ_ROOT_DIR)/code/transactive/app/cctv/video_switch_agent/protocols/MODBUS \
				      $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/video_switch_agent

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
