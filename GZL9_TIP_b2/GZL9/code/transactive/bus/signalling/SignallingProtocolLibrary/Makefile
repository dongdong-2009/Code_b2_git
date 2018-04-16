# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2010/08/12 10:00:53 $
# Last modified by:  $Author: builder $
#
PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/signalling/SignallingProtocolLibrary
PROJ_NAME	= TA_SignallingProtocolLibrary

PROJ_SOURCE	= Telegram.cpp \
		  TelegramException.cpp \
		  IncomingTelegramProcessor.cpp \
		  CommonTelegram.cpp \
		  BlockInfoTelegram.cpp \
		  DynamicTrainDataTelegram.cpp \
		  MessagePollTelegram.cpp \
		  ScadaInfoTelegram.cpp \
		  SignalStatusTelegram.cpp \
		  TrainLocationTelegram.cpp \
		  TrainCommonPisInfoTelegram.cpp \
		  TrainPisInfoTelegram.cpp \
		  TelegramFilter.cpp \
		  TelegramFilterHelper.cpp \
		  PlannedTelegramStart.cpp \
		  PlannedTelegramContent.cpp \
		  PlannedTelegramEnd.cpp \
		  PracticalTelegramStart.cpp \
		  PracticalTelegramContent.cpp \
		  PracticalTelegramEnd.cpp \
		  HelperLogs.cpp

		  
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
