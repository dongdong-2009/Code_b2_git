# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/modbus_comms/Makefile $
# @author:  Ripple
# @version: $Revision: #1 $
#
# Last modification: $DateTime: 2008/11/28 16:26:01 $
# Last modified by:  $Author: builder $

PROJ_ROOT_DIR	= ../../../..
PROJ_CODE_DIR	= transactive/bus/modbus_comms
PROJ_NAME	= TA_ModbusComms
PROJ_SOURCE	= ModbusException.cpp \
		  ModbusConnection.cpp \
		  ModbusTcpConnection.cpp \
		  ModbusSerialConnection.cpp \
    		  SerialPortException.cpp \
		  SerialPort.cpp \
		  CrcCalculator.cpp \
		  SteModbusTcpConnection.cpp
PROJ_TYPE	= lib
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

#BR TODO: Assuming that the following isn't needed. Should delete it once confirmed
#PROJ_CFLAGS = -DUNIX -DSYSV 

# Libraries that this library is dependent upon.
DEPENDENT_LIBRARIES = \
	$(PROJ_ROOT_DIR)/code/transactive/core/exceptions/IDL

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
