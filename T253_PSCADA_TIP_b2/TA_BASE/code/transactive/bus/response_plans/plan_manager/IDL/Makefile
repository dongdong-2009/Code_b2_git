# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/plan_manager/IDL/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2017/10/31 16:21:54 $
# Last modified by:  $Author: CM $
#
# Make file for the Plan Manager's IDL
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/response_plans/plan_manager/IDL
PROJ_NAME	= TA_PlanManagerCorbaDef
PROJ_IDL	= IPlanManagerCorbaDef.idl
PROJ_TYPE	= lib
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive 
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/PlanAgent/IDL

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
