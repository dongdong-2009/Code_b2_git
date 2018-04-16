# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/Makefile $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2008/12/31 11:14:06 $
# Last modified by:  $Author: builder $
#
PROJ_ROOT_DIR   = ../../../../..
PROJ_CODE_DIR   = transactive/core/data_access_interface/entity_access
PROJ_NAME   = TA_EntityAccess
PROJ_SOURCE = \
            PlanAgentData.cpp \
            AlarmAgentEntityData.cpp \
            VideoSwitchResource.cpp \
            Camera.cpp \
            VideoSwitchAgent.cpp \
            VideoInput.cpp \
            Sequence.cpp \
            RecordingUnit.cpp \
            Quad.cpp \
            BVSStage.cpp \
            VideoOutput.cpp \
            VideoOutputGroup.cpp \
            VideoMonitor.cpp \
            ConfigEntity.cpp \
            Console.cpp \
            Banner.cpp \
            ConsoleAccessFactory.cpp \
            EntityAccessFactory.cpp \
            EntityData.cpp \
            EntityHelper.cpp \
            HistoryViewerData.cpp \
            ScadaHistoryConfigData.cpp \
            ScadaHistoryAgentEntityData.cpp \
            NotificationAgentData.cpp \
            PlanManagerEntityData.cpp \
            Process.cpp \
            System.cpp \
            ControlStation.cpp \
            AlarmGUI.cpp \
	    PowerDemandEditorEntityData.cpp \
            PMSAgentEntityData.cpp \
            VirtualDataPointAgentEntityData.cpp \
            RTUEntityData.cpp \
            StationEntityData.cpp \
            StationECSAgentEntityData.cpp \
            StationSystemEntityData.cpp \
            StationSubSystemEntityData.cpp \
            EquipmentEntityData.cpp \
            DataPointEntityData.cpp \
            DataNodeEntityData.cpp \
            DataPointAccessFactory.cpp \
            DataNodeAccessFactory.cpp \
            DefaultEntity.cpp \
            DutyAgentEntityData.cpp \
            RightsAgentEntityData.cpp \
            MasterECSAgentEntityData.cpp \
            AlarmVolumeTestEntityType.cpp \
            EventViewer.cpp \
            DataNodeAgentEntityData.cpp \
            ConfigurationEditor.cpp \
            ECSManager.cpp \
            TISAgentEntityData.cpp \
            STISEntityData.cpp \
            TTISEntityData.cpp \
	    TTISManagerEntityData.cpp \
            RadioEntityData.cpp \
            RadioSessionEntityData.cpp \
            SchedulingAgentEntityData.cpp \
            StationPAAgentEntityData.cpp \
            MasterPAAgentEntityData.cpp \
            AbstractPAAgentEntityData.cpp \
            WILDAgentEntityData.cpp \
            AuthenticationAgentEntityData.cpp \
            SystemStatusAgentEntityData.cpp \
            SsaParamHelper.cpp \
            AtsAgentEntityData.cpp \
            RadioDirectoryEntityData.cpp \
            AlarmStoreEntityData.cpp \
            TrainAgentEntityData.cpp \
            TrainSettingsEntityData.cpp \
            ArchiveManagerEntityData.cpp \
            MmsAgentEntityData.cpp \
            MmsAlarmSubmitterEntityData.cpp \
            MmsJobRequestSubmitterEntityData.cpp \
            MmsPeriodicEntityData.cpp \
            MmsConnectionEntityData.cpp \
            MisAgentEntityData.cpp \
            MisAlarmSubmitterEntityData.cpp \
            MisJobRequestSubmitterEntityData.cpp \
            MisPeriodicEntityData.cpp \
            MisConnectionEntityData.cpp \
            DisplayManagerEntityData.cpp \
            DutyManagerEntityData.cpp \
            RadioProfileEntityData.cpp \
            RegionManagerEntityData.cpp \
            TisLogViewerEntityData.cpp \
            CallBannerPage.cpp \
            OnlinePrintingAgentEntityData.cpp \
            TrainCctvCameraEntityData.cpp \
            LessAgentEntityData.cpp \
            RadioStateSynchronisationEntityData.cpp \
            EquipmentStatusViewerEntityData.cpp \
            InspectorPanelEntityData.cpp \
            ScadaRootEntityData.cpp \
	    ServerEntityData.cpp \
            RadioGlobalEntityData.cpp \
	    AtsTwpEntityData.cpp \
	    AtsPsdEntityData.cpp \
	    TelephoneManagerProfileEntityData.cpp \
	    PidLogViewerEntityData.cpp \
            HierarchicalAlarmViewEntityData.cpp \
	    RadioMFTAgentEntityData.cpp \
		 AlarmRaiserEntityData.cpp


PROJ_TYPE       = lib
PROJ_INC_DIR    = ./src $(PROJ_ROOT_DIR)/code/transactive
PROJ_OUT_DIR    = $(PROJ_ROOT_DIR)/sdk/$(OPERATING_SYSTEM)_$(STRING_FLAG)$(RELEASE_FLAG)

PROJ_CFLAGS = -DTA_ASSERT_LOG=1
##############################################################
# The main include file that has all the wiz bang stuff for neato compiling
##############################################################

include $(PROJ_ROOT_DIR)/code/tools/build/CommonMakefile.mak

install:
        #Do Nothing
