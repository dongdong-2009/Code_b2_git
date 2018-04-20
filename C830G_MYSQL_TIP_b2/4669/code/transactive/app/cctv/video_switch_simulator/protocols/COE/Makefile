# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_simulator/protocols/COE/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $
#
# COE Simulator library Makefile.

PROJ_ROOT_DIR 	= ../../../../../../..
PROJ_CODE_DIR	= transactive/app/cctv/video_switch_simulator/protocols/COE
PROJ_NAME     	= TA_COESimulator

PROJ_SOURCE   	= COECommandMonitor.cpp \
                	COECommsObserver.cpp \
			COESimulator.cpp \
			COESimulatorCommandProcessor.cpp \
                	COEEquipmentState.cpp

PROJ_TYPE     = lib

PROJ_INC_DIR 	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive

PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBDIRS	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
			$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS 		= TA_GenericSimulator \
	  	    	TA_Utilities \
		    	TA_Version \
		    	TA_Thread \
		    	TA_Sockets \
		    	TA_Synchronisation \
		    	TA_Exceptions \
		    	TA_Utilities \
		    	clntsh
             
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)/code/transactive


##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
