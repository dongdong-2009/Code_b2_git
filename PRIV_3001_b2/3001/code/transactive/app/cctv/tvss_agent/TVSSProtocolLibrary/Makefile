# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/3001/transactive/app/ats/ATSSimulatorLibrary/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:12:57 $
# Last modified by:  $Author: grace.koh $

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/cctv/tvss_agent/TVSSProtocolLibrary
PROJ_NAME	= TA_TVSSProtocolLibrary
PROJ_SOURCE	= AgentModeMonitor.cpp \
		ITcpEvent.cpp \
		ITcpMethod.cpp \
		TVSSClearImageMethod.cpp \
		TVSSCommandResponseEventReader.cpp \
		TVSSEquipmentQueryMethod.cpp \
		TVSSEquipmentResponseEventReader.cpp \
		TVSSEvent.cpp \
		TVSSEventReader.cpp \
		TVSSMethod.cpp \
		TVSSQuadScreenMethod.cpp \
		TVSSSequencingScreenMethod.cpp \
		TVSSAcknowledgeMethod.cpp \
		TVSSSingleScreenMethod.cpp \
		TVSSTrainborneEventReader.cpp \
		TVSSTrainBorneHeartbeatMethod.cpp \
		crc16.cpp


PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#
# DEPENDENT_LIBRARIES = 
#				$(PROJ_ROOT_DIR)/core/message/IDL \

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
