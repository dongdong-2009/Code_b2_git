# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File$
# @author:  Ripple
# @version: $Revision$
#
# Last modification: $DateTime$
# Last modified by:  $Author$

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/ats/AtsAgent
PROJ_NAME	= AtsAgent
PROJ_SOURCE	=  \
		AddressUtil.cpp \
		AtcDataTable.cpp \
		AtsAgent.cpp \
		AtsCctvData.cpp \
		AtsDssData.cpp \
		AtsEcsData.cpp \
		AtsModbusTcpConnection.cpp \
		AtsPasData.cpp \
		AtsPsdData.cpp \
		AtsTisData.cpp \
		AtsTrainData.cpp \
		AtsTrainDataTimer.cpp \
		AtsTwpData.cpp \
		AtsWatchDogTable.cpp \
		AtsWildData.cpp \
		CctvBooleanContext.cpp \
		CctvRealNumberContext.cpp \
		DataManager.cpp \
		DataPointHelper.cpp \
		DataPointControlStateProcessor.cpp \
		DssTable.cpp \
		ElectricalSubsectionTable.cpp \
		IscsWatchDogTable.cpp \
		main.cpp \
		ModbusComms.cpp \
		OccUpdateProcessor.cpp \
		DoubleAtsFailureWorkItem.cpp \
		OperationModeManager.cpp \
		PlatformTable.cpp \
		PowerData.cpp \
		RawTable.cpp \
		SectorData.cpp \
		StationOccUpdateProcessor.cpp \
		TableManager.cpp \
		TrainCctvCamera.cpp \
		TrainTable.cpp \
		VersionGen.cpp \
		WashTable.cpp \
		WatchDogData.cpp \
		AtsServerLinkStatusUpdateThread.cpp \
		WildTable.cpp

PROJ_TYPE	= exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBDIRS	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
							  $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE)FLAG)

PROJ_LIBS     = \
		TA_CorbaUtil \
		TA_DataPointAgentCorbaDef_Impl \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_AtsDai \
		TA_GenericAgent \
		TA_EventTriggeredPlans \
		TA_PlanDAI \
		TA_ScadaProxyLibrary \
		TA_DataNodeLibrary \
		TA_DataPointLibrary \
		TA_DerivedDataPointDAI \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_MmsCorbaDef \
		TA_AtsAgentCorbaDef \
		TA_TrainCorbaProxy \
		TA_TrainCommonLibrary \
		TA_TrainAgentObjectNames \
		TA_TrainAgentCorba \
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
		TA_ModbusComms \
		TA_Sockets \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_MathEvaluation \
		TA_Synchronisation \
		TA_ocl \
		ACE \
		clntsh \
		nnz10

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/ats \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy

PROJ_CFLAGS = -DTA_ASSERT_LOG=1 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

