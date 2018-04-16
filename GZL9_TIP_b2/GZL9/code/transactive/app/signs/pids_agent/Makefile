# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/10/01 16:36:38 $
# Last modified by:  $Author: grace.koh $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/signs/pids_agent
PROJ_NAME	= PidsAgent

PROJ_SOURCE	= Exceptions.cpp \
		AgentModeMonitor.cpp \
		PIDSAdHocDisplayRequest.cpp \
		PIDSClearRequest.cpp \
		RawTable.cpp \
		PIDSCommunicationsHandler.cpp \
		PIDSManager.cpp \
		PIDSMessage.cpp \
		StatusMonitor.cpp \
		StringHelper.cpp \
		PIDSAgent.cpp \
		VersionGen.cpp \
		PIDSSigDisplayRequest.cpp \
		main.cpp

PROJ_TYPE	= exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
		TA_CorbaUtil \
		TA_PidsAgentCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_RightsLibrary \
		TA_AuthenticationLibrary \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_ModbusHelperLib \
		TA_ModbusComms \
		TA_Timers \
		TA_Thread \
		TA_GenericAgent \
		TA_Bus_Event \
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
		TA_Timers \
		TA_Utilities \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_Version \
		TA_MathEvaluation \
		TA_Synchronisation \
		clntsh \
		mysqlclient \
		TA_ocl \
		TA_mcl \
		ACE \
		iconv

DEPENDENT_LIBRARIES = \
                $(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \

PROJ_CFLAGS = -DTA_ASSERT_LOG=1 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
