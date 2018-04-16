# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/notification_service/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#

PROJ_ROOT_DIR   = ../../../..
PROJ_CODE_DIR   = transactive/app/notification_service
PROJ_NAME       = NotificationServiceAgent
PROJ_SOURCE     = NotificationAgent.cpp \
		NotificationServiceThread.cpp \
		NotificationStatisticsThread.cpp \
		Gateway.cpp \
		GatewayConsumer.cpp \
		Federator.cpp \
		FederatableEventChannel.cpp \
		NSALocator.cpp \
		VersionGen.cpp \
		main.cpp
PROJ_TYPE       = exe
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS       = \
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
		TA_mcl \
		TA_ocl \
		mysqlclient \
		clntsh \
		COSNotify4 \
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
		$(PROJ_ROOT_DIR)/code/transactive/core/timers \
		$(PROJ_ROOT_DIR)/code/transactive/bus/generic_agent \
	$(PROJ_ROOT_DIR)/code/transactive/bus/event \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/synchronisation \
	$(PROJ_ROOT_DIR)/code/transactive/core/utilities \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface \
	$(PROJ_ROOT_DIR)/code/transactive/core/uuid \
	$(PROJ_ROOT_DIR)/code/transactive/core/event \
	$(PROJ_ROOT_DIR)/code/transactive/core/ocl \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/plans \
	$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent \
	$(PROJ_ROOT_DIR)/code/transactive/core/threads \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/derived_datapoints \
	$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/entity_access \
	$(PROJ_ROOT_DIR)/code/transactive/core/alarm \
	$(PROJ_ROOT_DIR)/code/transactive/core/configuration_updates \
	$(PROJ_ROOT_DIR)/code/transactive/core/database \
	$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/event_triggered \
	$(PROJ_ROOT_DIR)/code/transactive/core/corba \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
