# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2015/02/09 23:14:15 $
# Last modified by:  $Author: hongzhi.zhang $
# Makefile for the VideoSwitchAgentDAI classes.
#

PROJ_ROOT_DIR 	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/video_switch_agent
PROJ_NAME		= TA_VideoSwitchAgentDAI

PROJ_SOURCE		= \
 ConfigVideoOutputGroup.cpp \
 DAIVideoOutputGroup.cpp \
 VideoOutputGroupAccessFactory.cpp \
 VideoOutputGroupHelper.cpp \
 ConfigSequence.cpp \
 Sequence.cpp \
 SequenceHelper.cpp \
 VideoOutputGroupHelper.cpp \
 CameraPresetsAccessFactory.cpp \
 CameraPresetsData.cpp \
 ConfigVideoStationId.cpp \
 ConfigVideoTriggeringEvent.cpp \
 VideoStationId.cpp \
 VideoStationIdAccessFactory.cpp \
 VideoTriggeringEvent.cpp \
 VideoStationIdHelper.cpp \
 VideoTriggeringEvent.cpp \
 VideoTriggeringEventAccessFactory.cpp \
 VideoTriggeringEventHelper.cpp \
 ConfigTVSSStationId.cpp \
 TVSSStationId.cpp \
 TVSSStationIdAccessFactory.cpp \
 TVSSStationIdHelper.cpp

PROJ_TYPE   	= lib
PROJ_INC_DIR    	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
