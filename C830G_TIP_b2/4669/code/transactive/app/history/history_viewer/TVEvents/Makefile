# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/TVEvents/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
PROJ_ROOT_DIR = ../../../..
PROJ_NAME = TVSendEvent
PROJ_SOURCE = sendDataPointStateUpdateEvent.cpp VersionGen.cpp MonitorInputThread.cpp
PROJ_TYPE	= exe
PROJ_OBJDIR = ./obj
PROJ_INCDIR = ./src

PROJ_INCDIRS = $(PROJ_ROOT_DIR) \
	$(PROJ_ROOT_DIR)/core/message/IDL/src \
	$(PROJ_ROOT_DIR)/core/message/types \
	$(PROJ_ROOT_DIR)/core/message/src \
	$(PROJ_ROOT_DIR)/core/corba/src \
	$(PROJ_ROOT_DIR)/core/corba/IDL/src \
	$(PROJ_ROOT_DIR)/core/utilities/src \
	$(PROJ_ROOT_DIR)/core/synchronisation/src \
	$(PROJ_ROOT_DIR)/core/threads/src \
	$(PROJ_ROOT_DIR)/core/exceptions/src \
	$(PROJ_ROOT_DIR)/cots/omniORB/omniORB_4.0.3/Solaris8/omni/include


PROJ_LIBS = \
	TA_CorbaUtil \
	TA_Naming \
	TA_NamingCorbaDef \
 	TA_Message \
	TA_Thread \
	TA_Synchronisation \
	TA_MessageCorbaDef \
	TA_Utilities \
	TA_Exceptions \
	DataAccess \
	clntsh \
	nnz10 \
	TAuuid \
	DataCollectionDmIf \
	TA_DataAccessInterface \
    TA_Version \
    TA_CorbaDef
	

PROJ_LIBDIRS = $(PROJ_ROOT_DIR)/core/message/obj \
	$(PROJ_ROOT_DIR)/core/message/IDL/obj \
	$(PROJ_ROOT_DIR)/core/utilities/obj \
	$(PROJ_ROOT_DIR)/core/threads/obj \
	$(PROJ_ROOT_DIR)/core/corba/obj \
	$(PROJ_ROOT_DIR)/core/corba/IDL/obj \
	$(PROJ_ROOT_DIR)/core/versioning/obj \
	$(PROJ_ROOT_DIR)/core/exceptions/obj \
	$(PROJ_ROOT_DIR)/core/naming/obj \
	$(PROJ_ROOT_DIR)/core/naming/IDL/obj \
	$(PROJ_ROOT_DIR)/core/synchronisation/obj \
	$(PROJ_ROOT_DIR)/core/database/obj \
	$(PROJ_ROOT_DIR)/core/data_access_interface/obj \
	$(PROJ_ROOT_DIR)/cots/oracle/920/solaris/lib \
	$(PROJ_ROOT_DIR)/core/uuid/obj \
	$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_dmif/obj

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/core/message/IDL \
	$(PROJ_ROOT_DIR)/core/exceptions \
	$(PROJ_ROOT_DIR)/core/message \
	$(PROJ_ROOT_DIR)/core/utilities \
	$(PROJ_ROOT_DIR)/core/corba \
	$(PROJ_ROOT_DIR)/core/database \
	$(PROJ_ROOT_DIR)/core/data_access_interface \
	$(PROJ_ROOT_DIR)/app/history/scada_historian/scada_historian_dmif
	


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
	#Do Nothing
