# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/response_plans/plan_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/response_plans/plan_agent
PROJ_NAME	= TA_PlanAgentLibrary
PROJ_SOURCE	= PlanAgentLibrary.cpp \
		  PlanAgentEntityNotFoundException.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
		      $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
		      $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL 

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
