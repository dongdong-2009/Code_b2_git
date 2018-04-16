# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/process_management/IDL/Makefile $ 
# Author:  Ripple
# Version: $Revision: #4 $
#
# Last modification: $DateTime: 2013/01/09 10:06:41 $
# Last modified by:  $Author: limin.zhu $
#
# Make file for the ProcessManagementCorbaDef classes.
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/process_management/IDL
PROJ_NAME	= TA_ProcessManagementCorbaDef
PROJ_IDL	= ProcessManagementDataDefinitionsCorbaDef.idl \
           	  IManagedApplicationCorbaDef.idl \
           	  IManagedAgentCorbaDef.idl \
           	  IManagedGUICorbaDef.idl \
           	  IManagedProcessCorbaDef.idl \
           	  ISystemControllerAdminCorbaDef.idl \
           	  IProcessManagerCorbaDef.idl \
           	  IProcessObserverCorbaDef.idl \
           	  IProcessSubjectCorbaDef.idl \
           	  IControlStationCorbaDef.idl \
   	     	  IHeartbeatReceiverCorbaDef.idl \
   	     	  IDllHostCorbaDef.idl \
		  IShutdownCorbaDef.idl
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
#Do Nothing
