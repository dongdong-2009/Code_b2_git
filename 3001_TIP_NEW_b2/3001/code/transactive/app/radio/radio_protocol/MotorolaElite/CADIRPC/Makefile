# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/RadioProtocol/MotorolaElite/CADIRPC/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2010/08/12 11:47:25 $
# Last modified by:  $Author: builder $
#
PROJ_ROOT_DIR = ../../../../../../..
PROJ_CODE_DIR	= transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC
PROJ_NAME = TA_CADIRPC
PROJ_TYPE	= lib
PROJ_INC_DIR    = . ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/bus/radio/radio_protocol/MotorolaElite/CADI

# The source files
#
PROJ_MV_C_SOURCE = CADICommandAdapter.c \
		CADIProtocol.c \
		cadcmd_xdr.c \
		cadev_svc.c \
		cadev_xdr.c \
		cadi_xdr.c \
		CADICommand.c \
		CADIEventReceiver.c

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
