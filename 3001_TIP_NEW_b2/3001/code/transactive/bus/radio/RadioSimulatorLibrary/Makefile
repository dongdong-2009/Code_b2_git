# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioSimulatorLibrary/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $
#
# Make file for the RadioSimulatorLibrary 

PROJ_ROOT_DIR 	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/radio/RadioSimulatorLibrary
PROJ_NAME     	= TA_RadioSimulatorLibrary

PROJ_SOURCE  	= RadioCommandMonitor.cpp \
                RadioCommsObserver.cpp \
				RadioSimulator.cpp \
				RadioSimulatorCommandProcessor.cpp \
				SimulatorParser.cpp \
				SimulatorHelper.cpp \
				SimulatorUseCase.cpp

PROJ_TYPE     	= lib

PROJ_INC_DIR 	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive

PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBDIRS	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
			$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	 	= TA_GenericSimulator \
	  	    	TA_Utilities \
		    	TA_Version \
		    	TA_Thread \
		    	TA_Sockets \
		    	TA_Synchronisation \
		    	TA_Exceptions \
		    	TA_Utilities \
		    	clntsh
             
PROJ_IDL_INC  	= $(PROJ_ROOT_DIR)/code/transactive

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################
include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

