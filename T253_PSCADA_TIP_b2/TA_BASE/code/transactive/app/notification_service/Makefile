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

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/app/notification_service
PROJ_NAME	= NotificationServiceAgent
PROJ_SOURCE	= NotificationAgent.cpp \
	 	  NotificationServiceThread.cpp \
	 	  NotificationStatisticsThread.cpp \
	 	  Gateway.cpp \
	 	  GatewayConsumer.cpp \
	 	  Federator.cpp \
		  FederatableEventChannel.cpp \
		  NSALocator.cpp \
	 	  VersionGen.cpp \
		  main.cpp
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
		  TA_CorbaDef \
		  TA_GenericAgent \
		  TA_Bus_Event \
		  TA_AuthenticationLibrary \
		  TA_AuthenticationCorbaDef \
		  TA_EventTriggeredPlans \
		  TA_PlanAgentLibrary \
		  TA_PlanAgentCorbaDef \
		  TA_PlanDAI \
		  TA_PlanAgentDataCorbaDef \
		  TA_PlanManagerCorbaDef \
		  TA_ConfigurationUpdates \
    		  TA_CorbaUtil \
		  TA_ManagedProcess \
		  TA_ProcessManagementCorbaDef \
		  TA_Alarm \
		  TA_Naming \
		  TA_NamingCorbaDef \
		  TA_Thread \
   		  TA_Message \
                  TA_EventProcessing \
				  TA_Event \
				  TA_EventAgentCorbaDef \
    		  TA_EntityAccess \
		  TA_Uuid \
		  TA_DAI \
		  TA_SystemControllerDAI \
		  TA_DerivedDataPointDAI \
    		  TA_DataAccess \
    		  TA_AlarmAgentCorbaDef \
                  TA_EntityStatusUpdateCorbaDef \
                  TA_AlarmRuleDAI \
    		  TA_MessageCorbaDef \
    		  TA_NamingCorbaDef \
		TA_Timers \
		  TA_Utilities \
		  TA_Exceptions \
                  TA_CommonExceptionsCorbaDef \
		  TA_Version \
		  mysqlcppconn \
		  TA_ocl \
		  mysqlclient \
    		  clntsh \
		  nnz11 \
		  COSNotify4 \
		  COSDynamic4 \
    		  AttNotification4 \
		  TA_Synchronisation \
        	  ACE

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent \
	$(PROJ_ROOT_DIR)/code/transactive/core/process_management \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/system_controller \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access/IDL \
        $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/alarm_rule \
	$(PROJ_ROOT_DIR)/code/transactive/core/message \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming \
		$(PROJ_ROOT_DIR)/code/transactive/core/versioning \
		$(PROJ_ROOT_DIR)/code/transactive/core/timers

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
