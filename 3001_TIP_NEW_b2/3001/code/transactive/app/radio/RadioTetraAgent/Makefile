# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_TIP/4677/transactive/app/radio/RadioServerAgent/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2009/12/15 19:31:45 $
# Last modified by:  $Author: grace.koh $
#
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/radio/RadioTetraAgent
PROJ_NAME	= RadioTetraAgent
PROJ_SOURCE	= \
		RadioServerAgent.cpp \
		RadioServerAgentCommandImpl.cpp \
		RadioServerAgentCommandServant.cpp \
		RadioServerAgentFramework.cpp \
		RadioServerAgentSummary.cpp \
		RadioSDS.cpp \
		RadioSDSSummary.cpp \
		RadioSDSHandler.cpp \
		RadioDpStatusUpdateThread.cpp \
		RadioStatusHandler.cpp \
		RadioServerMonitor.cpp \
		RadioProfileHandler.cpp \
		RadioResourceSync.cpp \
		RadioMFTResponse.cpp \
		RadioMFTHandler.cpp \
		MccProtocolHelper.cpp \
		RemoteAPI.cpp \
		VersionGen.cpp \
		RadioSDSLinkStatusThread.cpp \
		main.cpp

PROJ_TYPE	= exe
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/idl/transactive
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
PROJ_LIB_DIRS	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
		$(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


#
# Order is important here.  Specify with most dependent libraries at the bottom
#
PROJ_LIBS =	\
		TA_TrainCorbaProxy \
		TA_TrainCommonLibrary \
		TA_TrainAgentCorba \
		TA_TrainAgentObjectNames \
		TA_DataPointAgentCorbaDef_Impl \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_ScadaCommonLib \
		TA_ScadaProxyLibrary \
		TA_DataPointLibrary \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_SDTSProtocol \
		TA_CADIProtocol \
		TA_CADIRPC \
		TA_RadioTcpCommunication \
		TA_RadioMethodProtocol \
		TA_RadioPDSProtocol \
		TA_TrainAgentCorbaDef_4677 \
		TA_RightsLibrary \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_AuthenticationLibrary \
		TA_AuthenticationCorbaDef \
		TA_MmsCorbaDef \
		TA_DerivedDataPointDAI\
		TA_EventTriggeredPlans \
		TA_PlanDAI \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanManagerCorbaDef \
		TA_PlanAgentDataCorbaDef \
		TA_Alarm \
		TA_AlarmAgentCorbaDef \
                TA_EntityStatusUpdateCorbaDef \
                TA_Naming \
		TA_NamingCorbaDef \
		TA_Message \
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_MessageCorbaDef \
		TA_CorbaUtil \
		TA_Utilities \
		TA_Version \
		TA_Timers \
		TA_Thread \
		TA_Synchronisation \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_ConfigurationUpdates \
		TA_Utilities \
		TA_MathEvaluation \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationCorbaDef \
		TA_EntityAccess \
		TA_RadioDAI_4677 \
		TA_DAI \
                TA_AlarmRuleDAI \
                TA_DataAccess \
		TA_Exceptions \
		TA_RadioAgentCorbaDef \
		TA_RadioAgentCorbaDef_4669 \
		TA_CommonExceptionsCorbaDef \
		TA_CorbaDef \
		TA_Sockets \
		TA_Uuid \
		TA_ocl \
		TA_mcl \
		ACE \
		clntsh \
		mysqlclient 

DEPENDENT_LIBRARIES = \
                $(PROJ_ROOT_DIR)/code/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol \
                $(PROJ_ROOT_DIR)/code/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol \
                $(PROJ_ROOT_DIR)/code/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC \
		$(PROJ_ROOT_DIR)/code/transactive/app/radio/RadioTetraAgent/RadioMethodProtocol \
		$(PROJ_ROOT_DIR)/code/transactive/app/radio/RadioTetraAgent/RadioTcpCommunication \
		$(PROJ_ROOT_DIR)/code/transactive/app/radio/RadioTetraAgent/RadioPDSProtocol \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/radio \
		$(PROJ_ROOT_DIR)/code/transactive/bus/radio/radio_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/trains/train_agent/idl \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
