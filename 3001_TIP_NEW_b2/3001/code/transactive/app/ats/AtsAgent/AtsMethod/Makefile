# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP/3001/transactive/app/ats/AtsAgent/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2009/12/29 14:12:57 $
# Last modified by:  $Author: grace.koh $
#
# Makefile for AtsMethod classes.
#

PROJ_ROOT_DIR	= ../../../../../..
PROJ_CODE_DIR	= transactive/app/ats/AtsAgent/AtsMethod
PROJ_NAME	= TA_AtsMethod
PROJ_SOURCE	= AtsEvent.cpp \
   		AtsEventOa1Table.cpp \
		AtsEventReader.cpp \
		AtsEventTrainStatusReport.cpp \
		AtsITcpEvent.cpp \
		AtsITcpMethod.cpp \
		AtsMethodOA1Table.cpp \
		AtsMethodTrainStatusReport.cpp \
		FrameCheckSequence.cpp \


PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

