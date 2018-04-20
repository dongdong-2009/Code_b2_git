# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/tis_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2012/03/13 18:22:45 $
# Last modified by:  $Author: huang.wenbo $
#
# Make file for the TA_TISAgentDAI classes.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/tis_agent
PROJ_NAME	= TA_TISAgentDAI

PROJ_SOURCE	= StisTimeScheduleAccessFactory.cpp \
                StisTimeScheduleData.cpp


PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
