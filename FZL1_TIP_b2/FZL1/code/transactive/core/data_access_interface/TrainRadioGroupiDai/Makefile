# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/core/data_access_interface/TrainRadioGroupiDai $
# @author:  COMBIULDER
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2007/11/02 10:17:14 $
# Last modified by:  $Author:  $
#
# Make file for the TrainRadioGroupiDai classes.
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/core/data_access_interface/TrainRadioGroupiDai
PROJ_NAME	= TA_TrainRadioGroupiDai

PROJ_SOURCE	= TrainRadioGroup.cpp \
              TrainRadioGroupAccessFactory.cpp

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
