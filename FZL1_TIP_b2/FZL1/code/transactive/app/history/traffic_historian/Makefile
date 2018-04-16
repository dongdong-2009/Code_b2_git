# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR = ../../..
PROJ_NAME		= StatisticsAgent 

PROJ_SOURCE		= \
	DBConnection.cpp\
	DBHistory.cpp\
	GetDevices.cpp \
	Historian.cpp \
	HistorianProcess.cpp \
	HistorianMain.cpp \
	HistoryParams.cpp \
	TDMap.cpp \
	TrafficDetector.cpp \
	MonitorController.cpp \
	EventConsumer.cpp \
	SafeQueue.cpp \
        VersionGen.cpp

PROJ_TYPE	= exe
PROJ_OBJDIR = ./obj
PROJ_INCDIR = ./src

PROJ_INCDIRS = $(PROJ_ROOT_DIR)/core/corba/src \
		$(PROJ_ROOT_DIR)/core/utilities/src \
		$(PROJ_ROOT_DIR)/core/database/src \
		$(PROJ_ROOT_DIR)/core/process_management/src \
		$(PROJ_ROOT_DIR)/core/threads/src \
		$(PROJ_ROOT_DIR)/core/synchronisation/src \
		$(PROJ_ROOT_DIR)/core/process_management/process_management_dmif/src \
		$(PROJ_ROOT_DIR)/core/common/src \
		$(PROJ_ROOT_DIR)/bus/generic_driver/src \
		$(PROJ_ROOT_DIR)/core/corba_named_obj/src \
		$(PROJ_ROOT_DIR)/app/traffic_analysers/traffic_detector_agent/traffic_detector_agent_dmif/src \
		$(PROJ_ROOT_DIR)/core/entity_config/src \
		$(PROJ_ROOT_DIR)/app/history/traffic_historian/traffic_historian_dmif/src \
		$(PROJ_ROOT_DIR)/core/event/src


PROJ_LIBS = StatisticsDmIf TdDeviceDmIf GenericDriver ManagedProcessDmIf \
		MiSynchronisation MiThread CorbaUtil Event Alarm \
		AlarmProcessorDmIf EntityConfig \
		DataAccess MiUtil MiVersion CommonDmIf ManagedProcess \
		NamedObjRef

PROJ_LIBDIRS = $(PROJ_ROOT_DIR)/core/corba/obj \
		$(PROJ_ROOT_DIR)/core/utilities/obj \
		$(PROJ_ROOT_DIR)/core/database/obj \
		$(PROJ_ROOT_DIR)/core/process_management/obj \
		$(PROJ_ROOT_DIR)/core/threads/obj \
		$(PROJ_ROOT_DIR)/core/synchronisation/obj \
		$(PROJ_ROOT_DIR)/core/process_management/process_management_dmif/obj \
		$(PROJ_ROOT_DIR)/core/common/obj \
		$(PROJ_ROOT_DIR)/bus/generic_driver/obj \
		$(PROJ_ROOT_DIR)/core/corba_named_obj/obj \
		$(PROJ_ROOT_DIR)/app/traffic_analysers/traffic_detector_agent/traffic_detector_agent_dmif/obj \
		$(PROJ_ROOT_DIR)/core/entity_config/obj \
		$(PROJ_ROOT_DIR)/app/history/traffic_historian/traffic_historian_dmif/obj \
		$(PROJ_ROOT_DIR)/core/event/obj \
		$(PROJ_ROOT_DIR)/core/alarm/obj \
		$(PROJ_ROOT_DIR)/core/alarm/alarm_dmif/obj \
		$(PROJ_ROOT_DIR)/core/versioning/obj

#
# Libraries that this library is dependent upon.
#
# DEPENDENT_LIBRARIES
DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/core/corba \
		$(PROJ_ROOT_DIR)/core/utilities \
		$(PROJ_ROOT_DIR)/core/database \
		$(PROJ_ROOT_DIR)/core/process_management \
		$(PROJ_ROOT_DIR)/core/threads \
		$(PROJ_ROOT_DIR)/core/synchronisation \
		$(PROJ_ROOT_DIR)/core/process_management/process_management_dmif \
		$(PROJ_ROOT_DIR)/core/common \
		$(PROJ_ROOT_DIR)/bus/generic_driver \
		$(PROJ_ROOT_DIR)/core/corba_named_obj \
		$(PROJ_ROOT_DIR)/app/traffic_analysers/traffic_detector_agent/traffic_detector_agent_dmif \
		$(PROJ_ROOT_DIR)/core/entity_config \
		$(PROJ_ROOT_DIR)/app/history/traffic_historian/traffic_historian_dmif \
		$(PROJ_ROOT_DIR)/core/event \
		$(PROJ_ROOT_DIR)/core/alarm \
		$(PROJ_ROOT_DIR)/core/alarm/alarm_dmif \
		$(PROJ_ROOT_DIR)/core/versioning

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
	#Do Nothing

