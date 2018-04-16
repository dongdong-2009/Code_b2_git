# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $
PROJ_ROOT_DIR = ../../..

PROJ_NAME = DataRecordingAgent

PROJ_SOURCE = VersionGen.cpp \
	      DataPointData.cpp \
	      HistoryConfig.cpp \
	      DataPointCache.cpp \
	      MaximumConfig.cpp \
	      MinimumConfig.cpp \
	      AverageConfig.cpp \
	      SampleConfig.cpp \
	      DatapointLoaderThread.cpp \
	      ProcessTimerThread.cpp \
	      ScadaHistorianAgentProcess.cpp \
	      DataPointStateUpdateSubscriber.cpp \
	      DatapointOnlineUpdateSubscriber.cpp \
	      ScadaHistorianAgent.cpp \
	      main.cpp \
	      $(PROJ_ROOT_DIR)/../core/exceptions/src/EntityTypeException.cpp \
	      $(PROJ_ROOT_DIR)/../core/exceptions/src/DataConfigurationException.cpp \
	      $(PROJ_ROOT_DIR)/../core/exceptions/src/NoAlarmAgentException.cpp 

	      
PROJ_TYPE	= exe

PROJ_OBJDIR = ./obj

PROJ_INCDIR = ./src

PROJ_CFLAGS = -g -DTA_ASSERT_LOG=1

PROJ_INCDIRS = $(PROJ_ROOT_DIR) \
		$(PROJ_ROOT_DIR)/core/exceptions/src \
		$(PROJ_ROOT_DIR)/core/synchronisation/src \
		$(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef/src \
		$(PROJ_ROOT_DIR)/bus/generic_agent/src \
		$(PROJ_ROOT_DIR)/core/threads/src \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_dmif/src \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_data_access/src \
		$(PROJ_ROOT_DIR)/core/utilities/src \
		$(PROJ_ROOT_DIR)/core/corba/src \
		$(PROJ_ROOT_DIR)/core/versioning/src \
		$(PROJ_ROOT_DIR)/core/process_management/src \
		$(PROJ_ROOT_DIR)/core/process_management/IDL/src \
		$(PROJ_ROOT_DIR)/core/database/src \
		$(PROJ_ROOT_DIR)/core/data_access_interface/src \
		$(PROJ_ROOT_DIR)/core/data_access_interface/entity_access/src \
		$(PROJ_ROOT_DIR)/core/configuration_updates/src \
		$(PROJ_ROOT_DIR)/core/message/IDL/src \
		$(PROJ_ROOT_DIR)/core/message/src \
		$(PROJ_ROOT_DIR)/core/message/types \
                $(PROJ_ROOT_DIR)/cots/ACE/5_3/ACE_wrappers/ 
		

PROJ_LIBS = DataCollectionDmIf DataPointCorbaDef \
		HistorianDataAccess clntsh  TA_GenericAgent TA_EventTriggeredPlans \
		TA_Thread TA_CorbaUtil TA_Message TA_MessageCorbaDef TA_Synchronisation \
		TA_Alarm TA_AlarmAgentCorbaDef \
		TA_EntityAccess TA_DataAccessInterface \
		TA_ProcessManagement TA_ProcessManagementCorbaDef \
		TA_Naming TA_Utilities TA_Exceptions TA_CorbaDef TA_NamingCorbaDef \
		DataAccess TA_Version TAuuid \
	    TA_ConfigurationUpdates ACE

PROJ_LIBDIRS = $(PROJ_ROOT_DIR)/core/synchronisation/obj \
		$(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef/obj \
		$(PROJ_ROOT_DIR)/bus/generic_agent/obj \
		$(PROJ_ROOT_DIR)/core/threads/obj \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_dmif/obj \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_data_access/obj \
		$(PROJ_ROOT_DIR)/core/utilities/obj \
		$(PROJ_ROOT_DIR)/core/corba/obj \
		$(PROJ_ROOT_DIR)/core/corba/IDL/obj \
		$(PROJ_ROOT_DIR)/core/versioning/obj \
		$(PROJ_ROOT_DIR)/core/process_management/obj \
		$(PROJ_ROOT_DIR)/core/process_management/IDL/obj \
		$(PROJ_ROOT_DIR)/core/message/obj \
		$(PROJ_ROOT_DIR)/core/message/IDL/obj \
		$(PROJ_ROOT_DIR)/core/data_access_interface/obj \
		$(PROJ_ROOT_DIR)/core/data_access_interface/entity_access/obj \
		$(PROJ_ROOT_DIR)/core/naming/obj \
		$(PROJ_ROOT_DIR)/core/naming/IDL/obj \
		$(PROJ_ROOT_DIR)/core/exceptions/obj \
		$(PROJ_ROOT_DIR)/core/database/obj \
		$(PROJ_ROOT_DIR)/cots/oracle/920/solaris/lib \
		$(PROJ_ROOT_DIR)/core/uuid/obj \
		$(PROJ_ROOT_DIR)/core/alarm/obj \
		$(PROJ_ROOT_DIR)/core/alarm/IDL/obj \
		$(PROJ_ROOT_DIR)/core/configuration_updates/obj \
		$(PROJ_ROOT_DIR)/core/logging/obj \
                $(PROJ_ROOT_DIR)/cots/ACE/5_3/ACE_wrappers/ace

#
# Libraries that this library is dependent upon.
#
# DEPENDENT_LIBRARIES
DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/core/synchronisation \
		$(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef \
		$(PROJ_ROOT_DIR)/bus/generic_agent \
		$(PROJ_ROOT_DIR)/core/threads \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_dmif \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_data_access \
		$(PROJ_ROOT_DIR)/core/utilities \
		$(PROJ_ROOT_DIR)/core/corba \
		$(PROJ_ROOT_DIR)/core/versioning \
		$(PROJ_ROOT_DIR)/core/process_management \
		$(PROJ_ROOT_DIR)/core/database \
		$(PROJ_ROOT_DIR)/core/alarm/IDL \
		$(PROJ_ROOT_DIR)/core/message \
		$(PROJ_ROOT_DIR)/core/naming \
		$(PROJ_ROOT_DIR)/core/data_access_interface \
		$(PROJ_ROOT_DIR)/core/data_access_interface/entity_access \
		$(PROJ_ROOT_DIR)/core/uuid \
		$(PROJ_ROOT_DIR)/core/alarm \
		$(PROJ_ROOT_DIR)/core/configuration_updates \
		$(PROJ_ROOT_DIR)/core/logging



##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
	#Do Nothing
