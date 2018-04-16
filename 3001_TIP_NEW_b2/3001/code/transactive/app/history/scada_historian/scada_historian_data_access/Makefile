# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/scada_historian_data_access/Makefile $
# @author:  Ripple
# @version: $Revision: #2 $
#
# Last modification: $DateTime: 2012/02/06 16:15:14 $
# Last modified by:  $Author: haijun.li $

PROJ_ROOT_DIR = ../../../..

PROJ_NAME		= HistorianDataAccess
PROJ_SOURCE = ArrayInserter.cpp OCIInsertAccess.cpp \
		InsertDBError.cpp InsertDBAccess.cpp \
		InsertSavePtr.cpp OCIAccessExtended.cpp \

PROJ_TYPE	= lib
PROJ_OBJDIR = ./obj
PROJ_INCDIR = ./src

PROJ_INCDIRS = $(PROJ_ROOT_DIR) \
		$(PROJ_ROOT_DIR)/core/database/src \
		$(PROJ_ROOT_DIR)/app/history/scada_historian/src \
		$(PROJ_ROOT_DIR)/core/utilities/src \
		$(PROJ_ROOT_DIR)/core/synchronisation/src \
		$(PROJ_ROOT_DIR)/bus/scada/DataPointCorbaDef/src \
		$(PROJ_ROOT_DIR)/cots/oracle/920/solaris/inc \
                $(PROJ_ROOT_DIR)/cots/ACE/5_3/ACE_wrappers/

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
