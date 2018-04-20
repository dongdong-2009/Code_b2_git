# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Makefile $
# @author:  Ripple
# @version: $Revision: #4 $
#
# Last modification: $DateTime: 2009/09/23 13:09:47 $
# Last modified by:  $Author: builder $
#
# Make file for the Radio Tetra Agent 
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/app/radio/RadioTetraAgent
PROJ_NAME	= RadioTetraAgent
PROJ_SOURCE	= main.cpp \
                Radio.cpp \
                RadioResourceManager.cpp \
                RadioDirectory.cpp \
                RadioDirectoryServant.cpp \
                RadioDirectorySynchronisation.cpp \
                RadioConnectionManager.cpp \
		RadioCorbaTypeConverter.cpp \
                RadioServant.cpp \
                RadioSession.cpp \
                RadioSessionInitialisationThread.cpp \
                RadioSessionServant.cpp \
                RadioTCPConnection.cpp \
                RadioTetraAgent.cpp \
                RadioTransactionManager.cpp \
                RadioSessionException.cpp \
                RadioSessionNotInitialisedException.cpp \
                RadioMethodTimeoutException.cpp \
                RadioAuthorisationManager.cpp \
                RadioEventSubscriber.cpp \
                RadioBackgroundProcessor.cpp \
                RadioTaskFactory.cpp \
                RadioTask.cpp \
                RadioSubscriberActivityTask.cpp \
                RadioCallStackUpdateTask.cpp \
		RadioSessionTask.cpp \
                RadioHelper.cpp \
                RadioSessionHelper.cpp \
		RadioSubscriberMonitor.cpp \
		RadioGroupCallAckTask.cpp \
		RadioResetSessionTask.cpp \
		RadioSDSDetailsUpdateTask.cpp \
		RadioCircuitModeManager.cpp \
		RadioCircuitTransfer.cpp \
		RadioFullStateReceiver.cpp \
		RadioStateUpdateReceiver.cpp \
		RadioStateSynchronisationServant.cpp \
		RadioStateMessageConverter.cpp \
		RadioStateUpdateBroadcaster.cpp \
		RadioFallbackException.cpp \
		RadioResetConnectionTask.cpp \
		RadioDeleteReferenceTask.cpp \
		RadioAgentConstants.cpp \
		RadioConfigurationException.cpp \
		RadioConnectionException.cpp \
		RadioException.cpp \
		ObservedSocket.cpp \
		RadioTcpServer.cpp \
		RadioTimedEvent.cpp \
		RadioServerLinkStatusUpdateThread.cpp \
		RadioKeepAliveHandlerThread.cpp \
		RadioResetSocketTask.cpp \
		VersionGen.cpp

PROJ_TYPE     = exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
		TA_TrainCorbaProxy \
		TA_TrainCommonLibrary \
		TA_TrainAgentObjectNames \
		TA_TrainAgentCorba \
		TA_ScadaProxyLibrary \
		TA_DataPointAgentCorbaDef_Impl \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_DataPointLibrary \
		TA_DerivedDataPointDAI \
		TA_DataNodeLibrary \
		TA_ScadaCommonCorbaDef \
		TA_ScadaCommonLib \
		TA_MathEvaluation \
		TA_CorbaUtil \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_Utilities \
		TA_Version \
		TA_Timers \
		TA_Thread \
		TA_GenericAgent \
		TA_EventTriggeredPlans \
		TA_PlanAgentLibrary \
		TA_PlanAgentCorbaDef \
		TA_PlanDAI \
		TA_PlanAgentDataCorbaDef \
		TA_PlanManagerCorbaDef \
		TA_Message \
		TA_EventProcessing \
		TA_Alarm \
		TA_Naming \
		TA_Sockets \
		TA_Synchronisation \
		TA_CorbaDef \
		TA_AlarmAgentCorbaDef \
		TA_EntityStatusUpdateCorbaDef \
		TA_NamingCorbaDef \
		TA_MessageCorbaDef \
		TA_ManagedProcess \
		TA_ProcessManagementCorbaDef \
		TA_AuthenticationLibrary \
		TA_AuthenticationCorbaDef \
		TA_RadioAgentCorbaDef_4669 \
		TA_RightsLibrary \
		TA_AuthenticationLibrary \
		TA_DutyAgentCorbaDef \
		TA_SecurityUtilities \
		TA_AuthenticationCorbaDef \
		TA_RightsAgentCorbaDef \
		TA_EntityAccess \
		TA_RadioDAI \
		TA_DAI \
                TA_AlarmRuleDAI \
		TA_DataAccess \
		TA_ConfigurationUpdates \
		TA_Exceptions \
		TA_CommonExceptionsCorbaDef \
		TA_Utilities \
		TA_Uuid \
		TA_RadioProtocolLibrary \
		TA_EntityAccess \
		TA_ocl \
		clntsh \
		nnz10 \
		ACE
             
DEPENDENT_LIBRARIES	= $(PROJ_ROOT_DIR)/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary \
	                  $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy
 
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
