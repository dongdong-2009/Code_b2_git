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
PROJ_CODE_DIR	= transactive/bus/trains/TrainProtocolLibrary
PROJ_NAME	= TA_TrainProtocolLibrary

PROJ_SOURCE	= \
				AbstractCctvCommand.cpp \
				AbstractPecCommand.cpp \
				AbstractPecEvent.cpp \
				AbstractTableEvent.cpp \
				AbstractTtisCommand.cpp \
				AtcEvent.cpp \
				AudioCabToCabEvent.cpp \
				BadCommandEvent.cpp \
				CallTrainCommandReceivedEvent.cpp \
				CctvCarrierOffCommand.cpp \
				CctvCarrierOnCommand.cpp \
				CctvCommandReceivedEvent.cpp \
				CctvQuadCommand.cpp \
				CctvSequenceCommand.cpp \
				CctvSingleCommand.cpp \
				ChangeAreaCommand.cpp \
				ChangeAreaCommandReceivedEvent.cpp \
				ChangeModeCommand.cpp \
				ChangeoverStatusEvent.cpp \
				EndOfOccCallEvent.cpp \
				EndOfTtisDownloadEvent.cpp \
				EndOfVoiceCallCommand.cpp \
				FrameCheckSequence.cpp \
				Oa1Event.cpp \
				OccCallCommandReceivedCommand.cpp \
				OccCallResetCommand.cpp \
				PaAudioSwitchEvent.cpp \
				PaCommandReceivedEvent.cpp \
				PaContinueCommand.cpp \
				PaCyclicPreRecordedAnnouncementCommand.cpp \
				PaLiveAnnouncementCommand.cpp \
				PaPreRecordedAnnouncementCommand.cpp \
				PaReadyForLiveAnnouncementEvent.cpp \
				PaRequestForResetEvent.cpp \
				PaResetCommand.cpp \
				PaResetEvent.cpp \
				PaTrainMessageCompletedEvent.cpp \
				PecActivatedEvent.cpp \
				PecAnswerCommand.cpp \
				PecCommandReceivedEvent.cpp \
				PecContinueCommand.cpp \
				PecReadyForConversationEvent.cpp \
				PecRequestForResetEvent.cpp \
				PecResetCommand.cpp \
				PecResetEvent.cpp \
				PecSelectedByDriverEvent.cpp \
				RequestForCallTrainCommand.cpp \
				RequestForOccCallEvent.cpp \
				TestCallEvent.cpp \
				TestCallResultCommand.cpp \
				TimsCommand.cpp \
				TimsEvent.cpp \
				TrainEventFactoryImpl.cpp \
				TtdFrame.cpp \
				TtisCommandReceivedEvent.cpp \
				TtisFreeTextMessageCommand.cpp \
				TtisLibraryDownloadCommand.cpp \
				TtisLibraryUpgradeCommand.cpp \
				TtisPreStoredMessageCommand.cpp \
				TtisResetCommand.cpp \
				TtisScheduleDownloadCommand.cpp \
				TtisScheduleUpgradeCommand.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainTransactionModel

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

