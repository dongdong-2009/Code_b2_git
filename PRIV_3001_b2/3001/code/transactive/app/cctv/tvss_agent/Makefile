# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/tvss_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:12:57 $
# Last modified by:  $Author: grace.koh $
#
# Makefile for the Video Switch Agent
#

PROJ_ROOT_DIR 	= ../../../../..
PROJ_CODE_DIR	= transactive/app/cctv/tvss_agent
PROJ_NAME 		= TVSSAgent
PROJ_SOURCE 	= main.cpp \
              	TVSS.cpp \
              	TVSSAgent.cpp \
              	TVSSBackgroundProcessor.cpp \
              	TVSSHelper.cpp \
              	TVSSServant.cpp \
              	TVSSServerLinkStatusUpdateThread.cpp \
                TVSSAlarmProcessor.cpp \
                VersionGen.cpp \
                TrainBorneHeartBeatProcessor.cpp

PROJ_TYPE   	= exe
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/idl/transactive
PROJ_INC_DIR 	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive /u02/tabuild/3001_TIP/Base_3001/Base/code/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive

PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBDIRS	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
			$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
			
PROJ_LIBS 		= TA_TVSSManagerCorbaDef \
	TA_VideoSwitchAgentDAI \
	TA_VideoSwitchAgentAccess \
	TA_VideoSwitchAgentCorbaDef \
 TA_TCPCommunication  \
 TA_TVSSProtocolLibrary  \
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
 TA_AuthenticationCorbaDef \
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
 TA_Utilities \
 TA_Version \
 TA_ConfigurationUpdates \
 TA_Timers \
 TA_Thread \
 TA_Uuid \
 TA_ModbusComms \
 TA_Exceptions \
 TA_CommonExceptionsCorbaDef \
 TA_Sockets \
 TA_MathEvaluation \
 TA_Synchronisation \
 TA_mcl \
 TA_ocl \
 ACE \
 mysqlclient \
 clntsh 


DEPENDENT_LIBRARIES  = $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/video_switch_agent \
		       $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy \
                       $(PROJ_ROOT_DIR)/code/transactive/app/cctv/tvss_agent/TCPCommunication \
                       $(PROJ_ROOT_DIR)/code/transactive/app/cctv/tvss_agent/TVSSProtocolLibrary 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
