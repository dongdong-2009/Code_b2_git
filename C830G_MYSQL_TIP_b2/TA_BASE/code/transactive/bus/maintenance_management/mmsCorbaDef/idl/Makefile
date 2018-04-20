# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/maintenance_management/mmsCorbaDef/idl/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
# Make file for the MMS Corba Def classes.
#
PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/maintenance_management/mmsCorbaDef/idl
PROJ_NAME	= TA_MmsCorbaDef
PROJ_IDL	= \
		  IMmsGlobalInhibitCorbaDef.idl \
		  IMmsConnectionCorbaDef.idl \
		  IMmsAlarmSubmitterCorbaDef.idl \
		  IMmsJobRequestSubmitterCorbaDef.idl
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/alarm/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
