# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/database_status_monitor/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
# Make file for the database_status_monitor classes.
#

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/bus/database_status_monitor
PROJ_NAME	= TA_Dsm
PROJ_SOURCE	= SimpleDbStatusChecker.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES =   $(PROJ_ROOT_DIR)/code/transactive/core/corba/IDL \
			$(PROJ_ROOT_DIR)/code/transactive/bus/database_status_monitor/dsmCorbaDef/idl \

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
