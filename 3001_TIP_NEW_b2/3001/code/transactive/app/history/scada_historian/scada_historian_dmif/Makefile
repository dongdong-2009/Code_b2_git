# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/scada_historian_dmif/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $
PROJ_ROOT_DIR = ../../../..

PROJ_NAME		= DataCollectionDmIf
PROJ_IDL 	= DataCollectionDmIf.idl \
			  ScadaHistorianUpdateCorbaDef.idl \
			  ScadaHistorianProcessTimeUpdateCorbaDef.idl \
			  ScadaHistorianCacheUpdateCorbaDef.idl
			  

PROJ_IDL_INC = $(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef/src \
		-I$(PROJ_ROOT_DIR) # Had to add a -I because it didn't seem to add it
PROJ_TYPE	= lib
PROJ_OBJDIR = ./obj
PROJ_INCDIR = ./src

PROJ_INCDIRS = \
	$(PROJ_ROOT_DIR) \
	$(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef/src \
	$(PROJ_ROOT_DIR)/core/exceptions/src

#
# Libraries that this library is dependent upon.
#
# DEPENDENT_LIBRARIES
DEPENDENT_LIBRARIES = $(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/tools/build/CommonMakefile.mak


install:
	#Do Nothing
