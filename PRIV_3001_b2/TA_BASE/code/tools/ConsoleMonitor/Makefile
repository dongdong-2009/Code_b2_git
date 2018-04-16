# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/notification_service/Makefile $
# @author:  Ripple
# @version: $Revision: #3 $
#
# Last modification: $DateTime: 2012/02/10 14:06:39 $
# Last modified by:  $Author: huirong.luo $
#

PROJ_ROOT_DIR	= ../../..
PROJ_CODE_DIR	= tools/ConsoleMonitor
PROJ_NAME	= ConsoleMonitor
PROJ_SOURCE	= main.cpp \
	 	  VersionGen.cpp \
	 	  MainGuard.cpp \
	 	  CommandParserFactory.cpp \
	 	  CommonModuleCommandParser.cpp \
	 	  MonitorCommonDataType.cpp \
	 	  MonitorMessageHelper.cpp \
	 	  MonitorMessageProcessor.cpp \
	 	  MonitorMessageSender.cpp \
	 	  NSAModuleCommandParser.cpp
	 	  
PROJ_TYPE	= exe
PROJ_INC_DIR	= ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR	= $(PROJ_ROOT_DIR)/bin/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_LIBS	= \
		  TA_GenericAgent \
		  TA_Timers \
		  TA_EventTriggeredPlans \
		  TA_PlanAgentLibrary \
		  TA_PlanAgentCorbaDef \
		  TA_PlanDAI \
		  TA_PlanAgentDataCorbaDef \
		  TA_PlanManagerCorbaDef \
    		  TA_CorbaUtil \
		  TA_ManagedProcess \
		  TA_ProcessManagementCorbaDef \
		  TA_Alarm \
		  TA_Naming \
		  TA_NamingCorbaDef \
		  TA_Thread \
   		  TA_Message \
		  TA_CorbaDef \
                  TA_EventProcessing \
		  TA_ConfigurationUpdates \
    		  TA_EntityAccess \
		  TA_Uuid \
		  TA_DAI \
		  TA_SystemControllerDAI \
		  TA_DerivedDataPointDAI \
    		  TA_DataAccess \
    		  TA_AlarmAgentCorbaDef \
                  TA_EntityStatusUpdateCorbaDef \
                  TA_AlarmRuleDAI \
		  TA_MessageCorbaDef \
    		  TA_NamingCorbaDef \
		  TA_MyAgentInterfaceCorbaDef \
		  TA_Utilities \
		  TA_Exceptions \
                  TA_CommonExceptionsCorbaDef \
		  TA_Version \
		  TA_mcl \
		  TA_ocl \
		  mysqlclient \
    		  clntsh \
		  COSNotify4 \
    		  AttNotification4 \
		  TA_Synchronisation \
        	  ACE

##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
	#Do Nothing
