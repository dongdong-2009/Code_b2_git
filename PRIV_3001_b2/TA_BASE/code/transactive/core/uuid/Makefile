# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_3001/TA_BASE/transactive/core/uuid/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2018/03/14 13:51:20 $
# Last modified by:  $Author: lim.cy $
#
# This file builds the Miuuid library.
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/uuid
PROJ_NAME	= TA_Uuid
PROJ_SOURCE	= TAuuid.cpp
PROJ_MV_C_SOURCE = gen_uuid.c \
              	   pack.c \
              	   unpack.c \
              	   unparse.c
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

