# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/response_plans/plan_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2012/12/28 19:17:57 $
# Last modified by:  $Author: huangjian $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/response_plans/plan_agent
PROJ_NAME	= TA_PlanAgentLibrary
PROJ_SOURCE	= PlanAgentLibrary.cpp \
		  PlanAgentEntityNotFoundException.cpp \
		  SortableActiveStepId.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
		      $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_manager/IDL \
		      $(PROJ_ROOT_DIR)/code/transactive/bus/response_plans/plan_agent/IDL 

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
