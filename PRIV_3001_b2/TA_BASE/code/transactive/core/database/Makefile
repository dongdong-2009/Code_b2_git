# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/PRIV_3001/TA_BASE/transactive/core/database/Makefile $ 
# Author:  Ripple
# Version: $Revision: #1 $
#
# Last modification: $DateTime: 2018/03/14 13:51:20 $
# Last modified by:  $Author: lim.cy $
#
# Make file for the DataAccessLib classes.
#
#
PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/core/database
PROJ_NAME	= TA_DataAccess
PROJ_SOURCE	= DBException.cpp \
                  ConnectionReaper.cpp \
                  OCIConnection.cpp \
		  mysqlconnection.cpp \
                  DbServerLocator.cpp \
                  SimpleDb.cpp \
                  OCIQueryAdapter.cpp \
		  MysqlQueryAdapter.cpp \
                  OCIAccessStatusMonitor.cpp \
                  OCIConnectionStatusMonitor.cpp \
                  SimpleDbStatusMonitor.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
