# $Source: /cvs/1TS-9999-T/code/transactive/bus/pa/cachedmaps/Attic/Makefile,v $
# $Revision: 1.1.2.5 $
# $Date: 2006/06/29 07:44:31 $ 
# $Author: zhaohua $
#
# Make file for the PACachedMaps library
#
# History
#
# $Log: Makefile,v $
# Revision 1.1.2.5  2006/06/29 07:44:31  zhaohua
# Merged code from branch PRIV_CODE_4669_DDB_0607
#
# Revision 1.1.2.4  2005/09/06 11:15:23  seanl
# General merge from Combuilder private branch (CB 4669 27.3.6)
#
# Revision 1.1.2.2.18.2  2005/08/01 09:42:31  andrewdc
# Commbuilder Jul29 Release
#
# Revision 1.1.2.2.18.1  2005/07/08 18:52:50  markd
# *** empty log message ***
#
# Revision 1.1.2.3  2005/06/16 07:50:01  johnd
# TD 11048
#
# Revision 1.1.2.2  2005/01/25 07:15:11  tabuild
# SDK Upgrade.
#
# Revision 1.1.2.1  2005/01/21 12:06:42  julienc
# Initial import.
#
# Revision 1.1.2.2  2004/07/12 06:58:48  seanl
# Added PaStationCachedMap class
#
# Revision 1.1.2.1  2004/06/30 02:12:04  seanl
# Changes for shifting common code to bus
#
#
#

PROJ_ROOT_DIR	= ../../../../..
PROJ_CODE_DIR	= transactive/bus/pa/cachedmaps
PROJ_NAME	= TA_PACachedMaps

PROJ_SOURCE	= CachedMaps.cpp \
		GenericCachedMapBase.cpp \
		PaAtsTriggeredBroadcastCachedMap.cpp \
		PaDvaVersionCachedMap.cpp \
		PaStationDvaMessageCachedMap.cpp \
		PaTrainDvaMessageCachedMap.cpp \
		PaZoneCachedMap.cpp \
		PaZoneGroupCachedMap.cpp \
		PaStationCachedMap.cpp \
                TisCachedMaps.cpp

PROJ_TYPE	= lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive \
		$(PROJ_ROOT_DIR)/code/transactive/bus/pa/pa_agent/IDL/src \
		$(PROJ_ROOT_DIR)/sdk/include/transactive 
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/build/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
