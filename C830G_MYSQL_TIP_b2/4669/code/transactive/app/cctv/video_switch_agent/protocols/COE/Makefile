# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
# This file builds the TA_COESwitchAgent library.
#

PROJ_ROOT_DIR 		= ../../../../../../..
PROJ_CODE_DIR		= transactive/app/cctv/video_switch_agent/protocols/COE
PROJ_NAME 			= TA_COESwitchAgent

PROJ_SOURCE 		= COECamera.cpp \
	        	COECommunicationsHandler.cpp \
              		COECommunicationsChannel.cpp \
              		COEHelper.cpp \
              		ReceivedDataProcessor.cpp \
              		AbstractCOERequestCommand.cpp \
              		COEInterfaceFactory.cpp \
              		COESwitchManager.cpp \
              		COEVideoInput.cpp \
              		COEVideoOutput.cpp \
	        	COEQuad.cpp \
	        	COEBVSStage.cpp \
              		COESequence.cpp \
              		COERecordingUnit.cpp \
              		SetRecordingUnitModeCommand.cpp \
              		ControlSequencePlaybackCommand.cpp \
	        	SwitchQuadInputsCommand.cpp \
	        	SwitchInputToOutputCommand.cpp \
              		SummaryPollingCommand.cpp \
              		SetSequenceConfigCommand.cpp \
              		MonitorStatusCommand.cpp \
              		QuadStatusCommand.cpp \
              		StateValidationCycle.cpp \
              		SequenceConfigCommand.cpp \
			TrainCameraAlarmAckCommand.cpp \
			TrainCameraAlarmClearCommand.cpp \
              		TriggeringEventClearCommand.cpp \
              		TriggeringEventCommand.cpp \
              		TriggeringEventAckCommand.cpp \
              		RecordingUnitStatusCommand.cpp \
			BVSStatusCommand.cpp \
			SetStageSwitchModeCommand.cpp

PROJ_TYPE   		= lib
PROJ_INC_DIR 		= ./src $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)/transactive \
				$(PROJ_ROOT_DIR)/code/transactive \
				$(PROJ_ROOT_DIR)/sdk/include/transactive
				
PROJ_OUT_DIR		= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS			= TA_TrainAgentCorbaDef TA_VideoSwitchAgentCorbaDef

PROJ_LIBDIRS		= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
				$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
			
	
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
