# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/common_library/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
# This file builds the ScadaCommonLib library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/common_library
PROJ_NAME	= TA_ScadaCommonLib
PROJ_SOURCE	= AuditMessageUtility.cpp \
		  ScadaUtilities.cpp \
		  ScadaPersistence.cpp \
		  CobraCppConvertUtilities.cpp \
		  MmsScadaUtility.cpp \
		  DpValue.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_library \
	$(PROJ_ROOT_DIR)/code/transactive/bus/maintenance_management/mmsCorbaDef/idl \
	$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL
	
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
