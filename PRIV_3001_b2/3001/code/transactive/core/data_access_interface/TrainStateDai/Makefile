# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/core/data_access_interface/TrainStateDai/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2006/10/20 12:53:52 $
# Last modified by:  $Author: grace.koh $
# Makefile for the VideoSwitchAgentDAI classes.
#

PROJ_ROOT_DIR 	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/TrainStateDai
PROJ_NAME		= TA_TrainStateDai

PROJ_SOURCE		= \
                  PersistedTrainState.cpp \
                  PersistedTrainStateAccessFactory.cpp \
				  Train.cpp \
				  TrainHelper.cpp \
				  TrainAccessFactory.cpp 
                 

PROJ_TYPE   	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak


install:
        #Do Nothing
