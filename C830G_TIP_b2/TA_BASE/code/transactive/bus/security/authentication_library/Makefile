# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/authentication_library/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2009/01/02 14:05:26 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/security/authentication_library
PROJ_NAME	= TA_AuthenticationLibrary
PROJ_SOURCE	= AuthenticationLibrary.cpp \
		  KeepSessionAliveThread.cpp \
		  KeepSessionAliveThreadManager.cpp\
		  AuthenPeerTask.cpp\
		  AuthenPeerTaskManager.cpp\
		  AuthenPeerTaskMO.cpp\
		  GetActiveSessionsMO.cpp
		  
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/bus/security/authentication_agent/IDL \
	$(PROJ_ROOT_DIR)/code/transactive/core/naming/IDL

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
