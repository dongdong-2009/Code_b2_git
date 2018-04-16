# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/FZL1_TIP/FZL1/transactive/bus/bas/bas_agent_library/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/01/19 17:59:10 $
# Last modified by:  $Author: CM $
#
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/bas/bas_agent_library
PROJ_NAME	= TA_BasAgentLibrary

PROJ_SOURCE	= BasAgentProxy.cpp \
              BasAgentProxyFactory.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
 $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/bas \
 $(PROJ_ROOT_DIR)/code/transactive/bus/bas/bas_agent/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

