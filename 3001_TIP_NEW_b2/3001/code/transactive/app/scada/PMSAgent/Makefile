#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
#Source:    $Source: /cvs/1TS-9999-T/code/transactive/app/scada/PMSAgent/Attic/Makefile,v $
#@author:   HoaVu
#@version:  $Revision: #5 $
#Last modification: $Date: 2014/07/14 $
#Last modified by:  $Author: Noel $

PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/app/scada/PMSAgent
PROJ_NAME       = PMSAgent

PROJ_SOURCE     = PMSAgentMain.cpp \
				PMSAgent.cpp \
				RTU.cpp \
				RTUCommandWorker.cpp \
				RTUOutputWorker.cpp \
				RTUPollingWorker.cpp \
				RTUStatusWorker.cpp \
				RTUDownloadWorker.cpp \
				StationSystemOutputWriter.cpp \
				HeartBeatWorker.cpp \
				RTUManager.cpp \
				RTUDpProcessor.cpp \
				VersionGen.cpp

PROJ_TYPE       = exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
				TA_CorbaUtil \
				TA_DataPointAgentCorbaDef_Impl \
				TA_DataNodeAgentCorbaDef_Impl \
				TA_AlarmRuleDAI \
				TA_PlanDAI \
				TA_GenericAgent \
				TA_Bus_Event \
				TA_EventTriggeredPlans \
				TA_PlanAgentLibrary \
				TA_PlanAgentCorbaDef \
				TA_PlanDAI \
				TA_PlanAgentDataCorbaDef \
				TA_PlanManagerCorbaDef \
				TA_ScadaProxyLibrary \
				TA_DataNodeLibrary \
				TA_DataPointLibrary \
				TA_DerivedDataPointDAI \
				TA_ScadaCommonLib \
				TA_ScadaCommonCorbaDef \
				TA_DataRecording \
				TA_DataRecordingDAI \
				TA_MmsCorbaDef \
				TA_Alarm \
				TA_RightsLibrary \
				TA_AuthenticationLibrary \
				TA_SecurityUtilities \
				TA_AuthenticationCorbaDef \
				TA_RightsAgentCorbaDef \
				TA_AuthenticationCorbaDef \
				TA_DutyAgentCorbaDef \
				TA_Timers \
				TA_Thread \
				TA_Naming \
				TA_AlarmAgentCorbaDef \
				TA_EntityStatusUpdateCorbaDef \
				TA_Uuid \
				TA_Message \
				TA_EventProcessing \
				TA_Event \
				TA_EventAgentCorbaDef \
				TA_EntityAccess \
				TA_DAI \
				TA_AlarmRuleDAI \
				TA_CorbaDef \
				TA_NamingCorbaDef \
				TA_MessageCorbaDef \
				TA_ManagedProcess \
				TA_ProcessManagementCorbaDef \
				TA_DataAccess \
				TA_ConfigurationUpdates \
				TA_Utilities \
				TA_Version \
				TA_ModbusComms \
				TA_FTP \
				TA_Sockets \
				TA_Exceptions \
				TA_CommonExceptionsCorbaDef \
				TA_MathEvaluation \
				TA_Synchronisation \
				TA_ocl \
				TA_mcl \
				ACE \
				clntsh \
				mysqlclient 

PROJ_CFLAGS = -DTA_ASSERT_LOG=1

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
