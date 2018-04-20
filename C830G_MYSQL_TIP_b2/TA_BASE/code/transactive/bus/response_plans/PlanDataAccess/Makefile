# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/response_plans/PlanDataAccess/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2011/03/29 12:02:41 $
# Last modified by:  $Author: builder $
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/response_plans/PlanDataAccess
PROJ_NAME	= TA_PlanDataAccess
PROJ_SOURCE	= PlanDataReader.cpp PlanDataBaseHelper.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
