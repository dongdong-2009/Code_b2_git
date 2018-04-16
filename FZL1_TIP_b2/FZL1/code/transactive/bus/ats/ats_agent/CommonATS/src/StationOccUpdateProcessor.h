/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\StationOccUpdateProcessor.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This is a thread object responsible for calling the station SIG Agent for PA data synchronization
  */

#ifndef _STATION_OCC_UPDATE_PROCESSOR_H_
#define _STATION_OCC_UPDATE_PROCESSOR_H_

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/naming/src/INamedObject.h"
#include "core/naming/src/NamedObject.h"
#include "bus/ats/ats_agent/CommonATS/src/OccUpdateProcessor.h"
#include <vector>

namespace TA_IRS_Bus
{
	class StationOccUpdateProcessor : public TA_Base_Core::Thread
	{
	public:
		/**
		 * Constructor
		 */
		 StationOccUpdateProcessor(TA_Base_Core::CorbaName& corbaName, unsigned long stationId);
		 /**
		  * Destructor
		  */
		 virtual ~StationOccUpdateProcessor();

		 /**
          * run
          *
          * Method to start the thread
          */
		 virtual void run();

		  /**
          * terminate
          *
          * Method to terminate the thread
          */
		 virtual void terminate();

		 /**
          * processUpdates
          *
          * Method to process the data update
          */
		 void processUpdates();

		 /**
          * getStationId
          *
          * This method returns the Station ID
		  *
		  * @return long : StationID
          */
		 unsigned long getStationId();

		 /**
          * addPasUpdate
          *
          * This method adds PA data information
		  *
		  * @param pasMessage : PasMessage
          */
		 void addPasUpdate(const OccUpdateProcessor::PasMessage& pasMessage);

	private:
		// Disable default constructor, copy constructor and assignment operator
		StationOccUpdateProcessor();
		StationOccUpdateProcessor( const StationOccUpdateProcessor& occUpdateProcessor);
		StationOccUpdateProcessor& operator=(const StationOccUpdateProcessor &);

		 /**
          * initialize
          *
          * This method initializes corba object
		  *
          */
		void initialize();

		 /**
          * deInitialize
          *
          * This method deInitializes corba object
		  *
          */
		void deInitialize();

		TA_Base_Core::CorbaName	   m_corbaName;
		OccUpdateProcessor::AtsAgentNamedObject*       m_AtsAgent;
		TA_Base_Core::Semaphore	   m_semaphore;
		bool					   m_keepRunning;
		unsigned long			   m_stationId;

		OccUpdateProcessor::OccPasUpdates m_pasUpdates;
	};
}
#endif