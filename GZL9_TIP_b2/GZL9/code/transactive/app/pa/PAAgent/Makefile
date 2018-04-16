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
#
# Make file for the PA Agent 
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/pa/PAAgent
PROJ_NAME	= PAAgent
PROJ_SOURCE	= \
		AbstractBroadcast.cpp \
		AbstractBroadcastCircularList.cpp \
		AbstractNonTrainDVABroadcast.cpp \
		AbstractPAAgent.cpp \
		AbstractStationBroadcast.cpp \
		AbstractTrainBroadcast.cpp \
		AtsAgentSubscriber.cpp \
		BroadcastManager.cpp \
		BroadcastWorker.cpp \
		CachedConfig.cpp \
		DVARecordingBroadcast.cpp \
		MasterDVABroadcast.cpp \
		MasterLiveBroadcast.cpp \
		MasterPAAgent.cpp \
		PASConnection.cpp \
		ModbusPacket.cpp \
		FireAlarmSubscriber.cpp \
		TriggeredDVABroadcast.cpp \
		TriggeredBroadcastWorker.cpp \
		ScheduleBroadcast.cpp \
		ScheduleBroadcastWorker.cpp \
		DVABroadcastStart.cpp \
		DVABroadcastStop.cpp \
		DvaPlaybackStart.cpp \
		DvaPlaybackStop.cpp \
		LiveBroadcastStart.cpp \
		LiveBroadcastStop.cpp \
		ZoneMonitoringStart.cpp\
		ZoneMonitoringStop.cpp\
		BackgroundMusicStart.cpp\
		BackgroundMusicStop.cpp\
		PasHelpers.cpp \
		ProgressReportSentry.cpp \
		StationDVABroadcast.cpp \
		StationLiveBroadcast.cpp \
		StationMusicBroadcast.cpp \
		StationPAAgent.cpp \
		VersionGen.cpp \
		main.cpp

PROJ_TYPE	= exe
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/idl/transactive
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
PROJ_LIB_DIRS	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
		$(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


#
# Order is important here.  Specify with most dependent libraries at the bottom
#
PROJ_LIBS =	\
		TA_PAAgentCorbaDef \
		TA_PACachedMaps \
		TA_AtsAgentCorbaDef \
		TA_TISAgentAccess \
		TA_TISManagerCorbaDef \
		TA_TISAgentCorbaDef \
		TA_TrainCorbaProxy \
		TA_TrainCommonLibrary \
		TA_TrainAgentObjectNames \
		TA_TrainAgentCorba \
		TA_AuthenticationCorbaDef \
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
		TA_ModbusHelperLib \
		TA_ModbusComms \
		TA_DerivedDataPointDAI \
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
		TA_EventProcessing \
		TA_Event \
		TA_EventAgentCorbaDef \
		TA_MessageCorbaDef \
		TA_CorbaUtil \
		TA_EventScheduler \
		TA_Utilities \
		TA_Version \
		TA_Timers \
		TA_Thread \
		TA_Naming \
		TA_NamingCorbaDef \
		TA_Synchronisation \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_ConfigurationUpdates \
		TA_Utilities \
		TA_MathEvaluation \
		TA_DerivedDataPointDAI \
		TA_RightsLibrary \
		TA_AuthenticationLibrary \
		TA_ProcessManagementCorbaDef \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_AuthenticationCorbaDef \
		TA_PADAI \
		TA_EntityAccess \
		TA_DAI \
		TA_AlarmRuleDAI \
		TA_DataAccess \
		TA_Thread \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_CorbaDef \
		TA_Sockets \
		TA_Uuid \
		TA_ocl \
		TA_mcl \
		clntsh \
		mysqlclient \
		ACE 
 
DEPENDENT_LIBRARIES = \
                $(PROJ_ROOT_DIR)/code/transactive/bus/pa/cachedmaps \
                $(PROJ_ROOT_DIR)/code/transactive/bus/signs/tis_agent_access \
                $(PROJ_ROOT_DIR)/code/transactive/bus/event \
                $(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
                $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy \
                $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCommonLibrary
 
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
