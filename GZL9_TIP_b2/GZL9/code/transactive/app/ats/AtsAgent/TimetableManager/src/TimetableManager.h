#ifndef TIMETABLEMANAGER_H
#define TIMETABLEMANAGER_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "app/ats/AtsAgent/TimetableManager/src/TimetableService.h"
#include "app/ats/AtsAgent/TimetableManager/src/TimetableProcessor.h"


namespace TA_IRS_App{

	class TimetableManager{
	
	public:
		/**
          * Constructor
          */
		TimetableManager::TimetableManager(unsigned long TimetblCheckInterval, std::string TimetbleDirPath);
		
        /**
          * Destructor
          */
		~TimetableManager();

		/**
          * OnControlMode
          *
          * When the agent is in control mode OnControlMode is called
		  * to ensure the services managed by TimetableManager is set 
		  * to control Mode
          */
		void TimetableManager::OnControlMode();

		/**
          * OnMonitorMode
          *
          * When the agent is in monitor mode OnMonitorMode is called
		  * to ensure the services managed by TimetableManager is set 
		  * to monitor Mode
          */
		void TimetableManager::OnMonitorMode();

		/**
          * start
          *
          * To start TimetableService thread 
		  * 
          */
		void start();

	private:
		TimetableService* m_timetableService;
		TimetableProcessor* m_timetableProcessor;
	
	};
}

#endif