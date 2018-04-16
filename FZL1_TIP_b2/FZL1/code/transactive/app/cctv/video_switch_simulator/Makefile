# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_simulator/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2012/06/12 13:35:44 $
# Last modified by:  $Author: builder $
#
# Simulator Makefile.

PROJ_ROOT_DIR 	= ../../../../..
PROJ_CODE_DIR	= transactive/app/cctv/video_switch_simulator
PROJ_NAME     	= VideoSwitchSimulator

PROJ_SOURCE   	= main.cpp \
                	VersionGen.cpp \

PROJ_TYPE     	= exe

PROJ_INC_DIR 	= ./src $(PROJ_ROOT_DIR)/code/transactive $(PROJ_ROOT_DIR)/sdk/include/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)
PROJ_LIBDIRS	= $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG) \
			$(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS 		= TA_COESimulator \
			TA_GenericSimulator \
			TA_Utilities \
		    	TA_Version \
		    	TA_Thread \
		    	TA_Sockets \
		    	TA_Synchronisation \
		    	TA_Exceptions \
		    	TA_Utilities \
			clntsh
             
PROJ_IDL_INC  = $(PROJ_ROOT_DIR)/code/transactive

DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/code/transactive/app/cctv/video_switch_simulator/protocols/COE

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing

