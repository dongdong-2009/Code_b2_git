#The source code in this file is the property of
#Ripple Systems and is not for redistribution in any form.
#Source:    $Source: /depot/FZL1_Review/FZL1/code/transactive/app/bas/BASAgent/Makefile,v $
#@author:   HoaVu
#@version:  $Revision: #5 $
#Last modification: $Date: 2017/05/30 $
#Last modified by:  $Author: yong.liu $

PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/app/bas/BASAgent
PROJ_NAME       = BASAgent

PROJ_SOURCE     = BASAgentMain.cpp \
				BASAgent.cpp \
				BasCachedConfig.cpp \
				BasModeEquipment.cpp \
				StationModeManager.cpp \
				PeriodicThread.cpp \
				ModeExecutionException.cpp \
				FireTriggerBasEquipment.cpp \
				FireTriggerBasEquipmentManager.cpp \
				StationFireMonitorThread.cpp \
				BasAgentController.cpp \
				BasModeEquipmentFactory.cpp \
				TimetableManager.cpp \
				BasEventSubmitter.cpp \
				DataPointUpdateProcessor.cpp \
				VersionGen.cpp

PROJ_TYPE       = exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS     = \
				TA_ScadaAgent \
				TA_BASDAI \
				TA_BasAgentCorbaDef \
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
				TA_ScadaBusinessLibrary \
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
				TA_RTULib \
				TA_ModbusComms \
				TA_FTP \
				TA_Sockets \
				TA_Exceptions \
				TA_CommonExceptionsCorbaDef \
				TA_MathEvaluation \
				TA_Synchronisation \
				TA_ocl \
				TA_mcl \
				TA_ModbusHelperLib \
				ACE \
				clntsh \
				mysqlclient

DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/bas \
 $(PROJ_ROOT_DIR)/code/transactive/bus/bas/bas_agent/IDL


PROJ_CFLAGS = -DTA_ASSERT_LOG=1

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
