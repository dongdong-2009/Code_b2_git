# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/trains/TrainTransactionModel/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008-1-21 17:24:30 $
# Last modified by:  $Author: buildteam $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/trains/TrainTransactionModel
PROJ_NAME	= TA_TrainTransactionModel

PROJ_SOURCE	= \
			CoreTrainState.cpp \
			NonBlockingTrainTransaction.cpp \
			ParallelTrainTransaction.cpp \
			ScheduledTrainTransaction.cpp \
			StateChangeWorkItem.cpp \
			TrainCommand.cpp \
			TrainEvent.cpp \
			TrainEventWorkItem.cpp \
			TrainState.cpp \
			TrainStateLock.cpp \
			TrainStatusManager.cpp \
			TrainTimer.cpp \
			TrainTransaction.cpp \
			TrainTransactionManager.cpp\
			UnsolicitedTrainTimerManager.cpp
						
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCommonLibrary

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

