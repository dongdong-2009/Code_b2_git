# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/pa_4669/pa_agent/IDL/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/pa_4669/pa_agent/IDL
PROJ_NAME	= TA_PAAgentCorbaDef_4669

PROJ_IDL = IPAAgentCorbaDef.idl \
           IStationPaMethod.idl \
           ITrainPaMethod.idl

PROJ_TYPE       = lib
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
                $(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/core/message/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainAgentCorba/idl

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing


