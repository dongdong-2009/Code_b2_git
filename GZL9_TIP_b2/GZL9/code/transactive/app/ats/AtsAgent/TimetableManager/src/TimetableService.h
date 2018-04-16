#ifndef _TIMETABLESERVICE_H_
#define _TIMETABLESERVICE_H_

#include "core/threads/src/Thread.h"
#include "app/ats/AtsAgent/TimetableManager/src/TimetableHelper.h"
#include "app/ats/AtsAgent/TimetableManager/src/ITimeTableProcessor.h"


namespace TA_IRS_App{

	class TimetableService : public TA_Base_Core::Thread {
	
	public:
		
		/**
          * Constructor
          */
		TimetableService(ITimetableProcessor& m_timetableProcessor, 
			unsigned long ChkInterval, std::string DirPath);
		/**
          * Destructor
          */
		~TimetableService();

		virtual void run();
		virtual void terminate();

		/**
          * OnControlMode
          *
          * Set TimetableService to control mode
          */
		void OnControlMode();

		/**
          * OnMonitorMode
          *
          * Set TimetableService to montioe mode
          */
		void OnMonitorMode();

	private:

		/**
		 * processTimeTable
		 *
		 * This will process the Time table information 
		 */
		void processTimeTable();

		/**
		 * processPlanTimeTable
		 *
		 * This will process the Plan Time table information, read .csv file and update database
		 */
		void processPlanTimeTable(std::string & currentFolderPath);

		/**
		 * processPracticalTimeTable
		 *
		 * This will process the Practical Time table information, read .csv file and update database
		 */
		void processPracticalTimeTable(std::string & currentFolderPath);

		const unsigned long m_checkDirIntervalSec;
		const std::string m_DirPath;
		volatile bool m_keepRunning;
		volatile bool m_inControlMode;
		
		ITimetableProcessor& m_timetableProcessor;


	};

}

#endif