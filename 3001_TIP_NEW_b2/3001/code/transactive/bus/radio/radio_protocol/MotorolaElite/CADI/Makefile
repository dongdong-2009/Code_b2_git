# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4677_TIP/4677/transactive/bus/radio/RadioProtocol/MotorolaElite/CADI/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2009/12/15 19:31:45 $
# Last modified by:  $Author: grace.koh $
#
# This makefile is used to process the CADI .x files, and need to
# be run prior to the CADIRPC project compilation

PROJ_NAME = TA_CADIRPC_GEN
PROJ_TYPE = lib

CADI_DIR = ../../../../../app/radio/radio_protocol/MotorolaElite/CADIRPC/src

BUILD: all

all:
	rpcgen $(CADI_DIR)/cadcmd.x
	rpcgen $(CADI_DIR)/cadev.x
	rpcgen $(CADI_DIR)/cadi.x


install:
