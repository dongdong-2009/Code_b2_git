# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_TIP/4677/transactive/bus/radio/radio_agent/IDL/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2009/12/15 19:31:45 $
# Last modified by:  $Author: grace.koh $
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/radio/radio_agent/IDL
PROJ_NAME	= TA_RadioAgentCorbaDef

PROJ_IDL =  CommonRadioAgentIDL.idl \
	RadioCommonExceptionsIDL.idl \
	RadioServerAgentIDL.idl	\
	RadioMFTAgentCallbackIDL.idl \
	RadioMFTAgentCommandIDL.idl
	

PROJ_TYPE       = lib
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/idl/transactive
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing


