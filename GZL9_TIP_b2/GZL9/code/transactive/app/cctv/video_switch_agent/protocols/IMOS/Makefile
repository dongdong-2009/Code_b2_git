# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/08/13 14:55:34 $
# Last modified by:  $Author: huang.wenbo $
#
# This file builds the TA_IMOSSwitchAgent library.
#

PROJ_ROOT_DIR 		= ../../../../../../..
PROJ_CODE_DIR		= transactive/app/cctv/video_switch_agent/protocols/IMOS
PROJ_NAME 			= TA_IMOSSwitchAgent

PROJ_SOURCE 		= IMOSCamera.cpp \
					IMOSCommunicationsHandler.cpp \
              		IMOSHelper.cpp \
              		IMOSPDInterfaceFactory.cpp \
              		IMOSSwitchManager.cpp \
              		IMOSVideoInput.cpp \
              		IMOSVideoOutput.cpp \
					IMOSQuad.cpp \
              		IMOSSequence.cpp \
              		IMOSRecordingUnit.cpp
PROJ_TYPE   		= lib
PROJ_INC_DIR 		= ./src $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive \
				$(PROJ_ROOT_DIR)/code/transactive \
				$(PROJ_ROOT_DIR)/sdk/include/transactive
				
PROJ_OUT_DIR		= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS			= TA_TrainAgentCorbaDef TA_VideoSwitchAgentCorbaDef_gzl9

PROJ_LIBDIRS		= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
				$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
			
	
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
