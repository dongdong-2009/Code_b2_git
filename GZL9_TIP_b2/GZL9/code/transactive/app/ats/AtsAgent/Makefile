# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/AtsAgent/Makefile $
# @author:  Ripple
# @version: $Revision: #6 $
#
# Last modification: $DateTime: 2013/04/02 11:21:56 $
# Last modified by:  $Author: peter.wong $

PROJ_ROOT_DIR	= ../../../../..
PROJ_BASE_DIR	= ../../../../../../../Base_3002/Base
PROJ_CODE_DIR	= transactive/app/ats/AtsAgent
PROJ_NAME	= AtsAgent
PROJ_SOURCE	=  \
		AtsAgent.cpp \
		main.cpp \
		VersionGen.cpp 
	
PROJ_TYPE	= exe
PROJ_INC_DIR    = $(PROJ_BASE_DIR)/sdk/Base/build/SOLARIS_n/transactive \
                  ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive 
                  
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIB_DIRS	= $(PROJ_BASE_DIR)/sdk/Base/sdk/SOLARIS_nd \
			$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
			$(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE)FLAG \
			$(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/lib/SOLARIS_5.11_sun4/lib

						  
PROJ_LIBS     = \
		TA_ScadaAgent \
		TA_GenaProtocolHandler \
		TA_GenaLibrary \
		TA_GenaDAI \
		TA_TimetableManager \
		TA_CommonATS \
		TA_CorbaUtil \
		TA_ItaScada \
		TA_DataPointAgentCorbaDef_Impl \
		TA_DataNodeAgentCorbaDef_Impl \
		TA_AtsDai \
		TA_GenericAgent \
		TA_Bus_Event \
		TA_EventTriggeredPlans \
		TA_PlanDAI \
		TA_ScadaProxyLibrary \
		TA_ScadaBusinessLibrary \
        TA_ModbusHelperLib \
		TA_DataNodeLibrary \
		TA_DataPointLibrary \
		TA_DerivedDataPointDAI \
		TA_ScadaCommonLib \
		TA_ScadaCommonCorbaDef \
		TA_DataRecording \
		TA_DataRecordingDAI \
		TA_MmsCorbaDef \
		TA_AtsAgentCorbaDef \
		TA_PidsAgentCorbaDef \
		TA_TrainCorbaProxy \
		TA_TrainCommonLibrary \
		TA_TrainAgentObjectNames \
		TA_TrainAgentCorba \
		TA_Alarm \
		TA_RightsLibrary \
		TA_RTULib \
		TA_FTP \
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
		TA_mcl \
		ACE \
		clntsh \
		mysqlclient \
		PocoUtil \
		PocoNet \
		PocoXML \
		PocoFoundation

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/ats \
		$(PROJ_ROOT_DIR)/code/transactive/bus/ats/ats_agent/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCorbaProxy \
		$(PROJ_ROOT_DIR)/code/transactive/app/ats/AtsAgent/GenaProtocolHandler \
		$(PROJ_ROOT_DIR)/code/transactive/app/ats/AtsAgent/TimetableManager

		
PROJ_CFLAGS = -DTA_ASSERT_LOG=1 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

STD_INC_DIRS += $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Foundation/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Util/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/XML/include \
 $(PROJ_ROOT_DIR)/code/cots/poco/1.4.6p4/Net/include \

install:
	#Do Nothing

