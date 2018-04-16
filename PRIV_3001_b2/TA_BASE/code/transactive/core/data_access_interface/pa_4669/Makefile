# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/pa_4669/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2018/03/14 13:51:20 $
# Last modified by:  $Author: lim.cy $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/pa_4669
PROJ_NAME	= TA_PADAI_4669


PROJ_SOURCE	= \
ConfigPaAtsTriggeredBroadcast.cpp \
ConfigPaDvaMessage.cpp \
ConfigPaTisMessageMap.cpp \
ConfigPaTisZoneMap.cpp \
ConfigPaDvaMessageVersion.cpp \
ConfigPaStation.cpp \
ConfigPaTrainDvaMessage.cpp \
ConfigPaZone.cpp \
ConfigPaZoneGroup.cpp \
DatabaseQueryHelper.cpp \
PaAtsTriggeredBroadcast.cpp \
PaAtsTriggeredBroadcastAccessFactory.cpp \
PaAtsTriggeredBroadcastHelper.cpp \
PaTisMessageMap.cpp \
PaTisZoneMap.cpp \
PaTisMessageMapHelper.cpp \
PaTisZoneMapHelper.cpp \
PaDvaMessage.cpp \
PaTisMessageMappingAccessFactory.cpp \
PaTisZoneMappingAccessFactory.cpp \
PaDvaMessageAccessFactory.cpp \
PaDvaMessageHelper.cpp \
PaDvaMessageVersion.cpp \
PaDvaMessageVersionAccessFactory.cpp \
PaPagingConsoleAccessFactory.cpp \
PaFireCountdownAccessFactory.cpp \
PaSysParamAccessFactory.cpp \
PaDvaMessageVersionHelper.cpp \
PaStation.cpp \
PaStationAccessFactory.cpp \
PaStationHelper.cpp \
PaTrainDvaMessage.cpp \
PaTrainDvaMessageAccessFactory.cpp \
PaTrainDvaMessageHelper.cpp \
PaZone.cpp \
PaZoneAccessFactory.cpp \
PaZoneGroup.cpp \
PaZoneGroupAccessFactory.cpp \
PaZoneGroupHelper.cpp \
PaZoneHelper.cpp \
ConfigPaScheduleBroadcast.cpp \
PaScheduleBroadcast.cpp \
PaScheduleBroadcastAccessFactory.cpp \
PaScheduleBroadcastHelper.cpp

		  
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/build/SOLARIS_n/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= TA_DAI

DEPENDENT_LIBRARIES = \
    	$(PROJ_ROOT_DIR)/code/transactive/core/event/IDL

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
