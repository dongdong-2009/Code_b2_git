# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2017/10/31 16:21:54 $
# Last modified by:  $Author: CM $
#
# This file builds the DataNodeAgentCorbaDef_Impl library.
#
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/DataNodeAgentCorbaDef_Impl
PROJ_NAME	= TA_DataNodeAgentCorbaDef_Impl
PROJ_SOURCE	= DataNodeAgentCorbaDef_Impl.cpp \
		  DataNodeAccessCorbaDef_Impl.cpp \
		  DataNodeObserver.cpp \
		  DataNodeStateContainer.cpp \
		  DataNodeAgentSynHandler.cpp \
		  DataNodeDisplayClient.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
