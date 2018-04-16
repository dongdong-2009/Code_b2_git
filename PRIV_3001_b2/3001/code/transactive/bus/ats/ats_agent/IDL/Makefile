# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/PRIV_3001/3001/transactive/bus/ats/ats_agent/IDL/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2018/03/14 13:51:20 $
# Last modified by:  $Author: lim.cy $
PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/bus/ats/ats_agent/IDL
PROJ_NAME	= TA_AtsAgentCorbaDef

PROJ_IDL	= AtsCommonCorbaDef.idl IAtsEcsCorbaDef.idl IAtsPasCorbaDef.idl IAtsTisCorbaDef.idl IAtsTrainCorbaDef.idl \
		IAtsWildCorbaDef.idl IAtsCctvCorbaDef.idl IAtsPsdCorbaDef.idl IAtsCorbaDef.idl  

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


:
