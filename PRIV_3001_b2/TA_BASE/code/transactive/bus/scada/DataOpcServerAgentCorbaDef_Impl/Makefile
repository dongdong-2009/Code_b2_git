# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/Makefile $
# @author:  Dhanshri Mokashi
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2016/06/27 15:58:07 $
# Last modified by:  $Author: Dhanshri Mokashi $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/scada/DataOpcServerAgentCorbaDef_impl
PROJ_NAME	= TA_DataOpcServerAgentCorbaDef_Impl
PROJ_SOURCE	= OpcSimpleNodeAgentCorbaDef_Impl.cpp \
		  SimpleNode.cpp \
		  SimpleNodeObserver.cpp \
		  SimpleNodeStateContainer.cpp \
		  SimpleNodeDisplayClient.cpp \
		  OpcSimplePointAgentCorbaDef_Impl.cpp \
		  SimplePoint.cpp \
		  SimplePointObserver.cpp \
		  SimplePointStateContainer.cpp \
		  SimplePointDisplayClient.cpp \
		  VersionGen.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
		
DEPENDENT_LIBRARIES = \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/common_library/IDL \
		$(PROJ_ROOT_DIR)/code/transactive/bus/scada/datanode_library \
		$(PROJ_ROOT_DIR)/code/transactive/core/data_access_interface 

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
