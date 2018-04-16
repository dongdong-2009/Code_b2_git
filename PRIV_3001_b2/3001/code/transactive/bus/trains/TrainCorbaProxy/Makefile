# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/trains/TrainCorbaProxy/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2008-1-21 17:24:30 $
# Last modified by:  $Author: buildteam $
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/trains/TrainCorbaProxy
PROJ_NAME	= TA_TrainCorbaProxy

PROJ_SOURCE	= \
				CallTypesConversion.cpp \
				CctvTypesConversion.cpp \
				CommonTypesConversion.cpp \
				PaTypesConversion.cpp \
				PecTypesConversion.cpp \
				RadioGroupTypesConversion.cpp \
				StateUpdateTypesConversion.cpp \
				TisTypesConversion.cpp \
				TrainAgentProxyFactory.cpp \
				TrainCctvCorbaProxy.cpp \
				TrainDriverCallCorbaProxy.cpp \
				TrainInformationConversion.cpp \
				TrainInformationCorbaProxy.cpp \
				TrainOperationModeCorbaProxy.cpp \
				TrainPaCorbaProxy.cpp \
				TrainPecCorbaProxy.cpp \
				TrainRadioGroupCorbaProxy.cpp \
				TrainStateUpdateCorbaProxy.cpp \
				TrainTisCorbaProxy.cpp

						
PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive \
				  $(PROJ_ROOT_DIR)/sdk/include/transactive/bus/trains/TrainAgentCorba/idl/src/ \
				  $(PROJ_ROOT_DIR)/sdk/include/transactive/bus/radio/RadioAgentCorbaDef_4669/idl/src
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/bus/trains/TrainCommonLibrary

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
	#Do Nothing

