# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/signs_4669/TisManagerCorbaDef/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $
#
# Makefile for the TisAgent IDL classes.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/signs_4669/TisManagerCorbaDef
PROJ_NAME	= TA_TISManagerCorbaDef_4669

PROJ_IDL	= \
		ISTISManagerCorbaDef.idl \
		ITTISManagerCorbaDef.idl

PROJ_TYPE	= lib
PROJ_IDL_INC	= $(PROJ_ROOT_DIR)/code/transactive
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface/tis_agent_4669/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

