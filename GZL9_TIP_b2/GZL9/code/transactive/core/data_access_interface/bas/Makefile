# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/FZL1_Review/FZL1/transactive/core/data_access_interface/bas/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2015/02/27 17:12:34 $
# Last modified by:  $Author: CM $


PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/bas
PROJ_NAME	= TA_BASDAI

PROJ_SOURCE	= \
			Mode.cpp \
			ModeAccessFactory.cpp \
			ModeHelper.cpp \
			ModeTypeAccessFactory.cpp \
			Subsystem.cpp \
			SubsystemAccessFactory.cpp \
			SubsystemHelper.cpp \
			ModeDeviceAccessFactory.cpp \
			SubsystemModetypeMappingAccessFactory.cpp \
			BASTimeTableMappingAccessFactory.cpp \
			BASTimeTableModeTimeAccessFactory.cpp \
			BASTimeTableTimeTableAccessFactory.cpp \
			BasParameterValueType.cpp \
			BasStationSectionAccessFactory.cpp \
			BasTunnelCongestionModeAccessFactory.cpp \
			BasTunnelFireModeAccessFactory.cpp \
			BasMileageAccessFactory.cpp \
			BasTrackAccessFactory.cpp \
			BasTunnelFireModeAccessFactory.cpp \
			BasFireAccessFactory.cpp\

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
