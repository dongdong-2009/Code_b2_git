# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/RadioProtocol/MotorolaElite/SDTSProtocol/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2010/08/12 11:47:25 $
# Last modified by:  $Author: builder $
#
PROJ_ROOT_DIR = ../../../../../../..
PROJ_CODE_DIR	= transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol
PROJ_NAME = TA_SDTSProtocol
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

# The source files
#
PROJ_SOURCE = CM_Broadcast.cpp \
		RadioSDSSocket.cpp \
		CM_Data.cpp \
		CM_RegisterRequest.cpp \
		CM_RegisterResponse.cpp \
		CM_ReportEvent.cpp \
		CMCommon1.cpp \
		CMCommon2.cpp \
		CMCommon3.cpp \
		msgDataFormat_TextMsg.cpp \
		msgDataFormat_TrainborneSDS.cpp \
		SDSAckRecvText.cpp \
		SDSAckSendText.cpp \
		SDSEvent.cpp \
		SDSFunction.cpp \
		SDSInputQueue.cpp \
		SDSOutputQueue.cpp \
		SDSQueueManager.cpp \
		SDSRecvText.cpp \
		SDSRegisterSession.cpp \
		SDSResponse.cpp \
		SDSSendText.cpp \
		SDSUnregisterSession.cpp \
		SDTSHelper.cpp \
		SDTSMsgFactory.cpp \
		SDTSProtocolEvent.cpp \
		SDTSTransactionManager.cpp \
		TL_DataEvent.cpp \
		TL_ReportEvent.cpp \
		TL_ShortReportEvent.cpp \
		TL_UnitDataEvent.cpp \
		TLCommon.cpp \


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
