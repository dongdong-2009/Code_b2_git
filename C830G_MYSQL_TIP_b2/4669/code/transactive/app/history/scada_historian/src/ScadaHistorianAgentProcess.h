#ifndef SCADAHISTORIANAGENTPROCESS_H
#define SCADAHISTORIANAGENTPROCESS_H

/**
  * ScadaHistorianAgentProcess.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/src/ScadaHistorianAgentProcess.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main processing class that starts off threads
  * to load, process and subscribe to messages.
  *
  */

#include <vector>
#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

using TA_Core::IEntityData;

namespace TA_App
{
    //
    // Forward declarations
    //
    class DatapointLoaderThread;
	class ProcessTimerThread;
    class DataPointStateUpdateSubscriber;
    class DataPointOnlineUpdateSubscriber;
    class HistoryConfig;

    class ScadaHistorianAgentProcess : public virtual TA_Core::Thread
    {
    public:

        /**
          * ScadaHistorianAgentProcess
          *
          * Constructor.
		  *
          */
   	    ScadaHistorianAgentProcess();

        /**
          * ~ScadaHistorianAgentProcess()
          *
          * Destructor.
          *
          */
	    virtual ~ScadaHistorianAgentProcess();

        /**
          * run
          *
          * Starts off the processing threads then just waits for events.
		  *
          */
        void run();

        /**
          * terminate
          *
          * Terminates the thread.
		  *
          */
        void terminate();

 	    /**
          * processUpdatedDataPoint
          *
          * This updates the cache with the new datapoint information.
		  * 
		  * @param entitykey Pkey of the updated datapoint
		  * @param dpState	 The structure that holds the updated information
		  *
          */
        void processUpdatedDataPoint(unsigned long entitykey, const TA_Bus::DataPointCorbaDef::ClientDataPointState dpState);

		/**
          * addHistoryConfig
          *
          * Stores pointers to the History Configs for processing.
		  *
		  * @param entitykey Pkey of the updated datapoint
		  *
          */
        void addHistoryConfig(HistoryConfig* historyConfig);

        /**
          * setToControl
          *
          * Alerts all threads that agent has changed operation modes.
		  *
		  * @param isControl True: Control mode, False: Monitor mode
		  *
          */
        void setToControl(bool isControl);

        /**
          * processUpdateCfgEvent
          *
          * Process a Modify Online Update for a datapoint
		  *
		  * @param entitykey Pkey of the entity changed
		  *
          */
        void processUpdateCfgEvent(unsigned long entitykey);

        /**
          * processCreateCfgEvent
          *
          * Process a Create Online Update for a datapoint
		  *
		  * @param entitykey Pkey of the entity changed
          *
          */
        void processCreateCfgEvent(unsigned long entitykey, IEntityData* entityData);

        /**
          * processDeleteCfgEvent
          *
          * Process a Delete Online Update for a datapoint
		  *
		  * @param entitykey Pkey of the entity changed
          *
          */
        void processDeleteCfgEvent(unsigned long entitykey);

        /**
          * addDataPointSubscriber
          *
          * Add a state update subscriber for a datapoint
		  *
		  * @param entitykey Pkey of the datapoint
          *
          */
        void addDataPointSubscriber(unsigned long entitykey);

        /**
          * removeDataPointSubscriber
          *
          * Remove a state update subscriber for a datapoint
		  *
		  * @param entitykey Pkey of the datapoint
          *
          */
        void removeDataPointSubscriber(unsigned long entitykey);
        
    private:

        void checkEntityParameters(IEntityData* entityData);

        std::string uLongToString(unsigned long ulong);
	    
        DatapointLoaderThread* m_datapointLoaderThread;

		ProcessTimerThread* m_processTimerThread;

        std::vector<DataPointStateUpdateSubscriber*> m_dataPointStateUpdateSubscribers;

        DataPointOnlineUpdateSubscriber* m_dataPointOnlineUpdateSubscriber;

        // PW 3374: Use a semaphore instead of looping continually to check
        // termination flag. Termination flag has been removed.
        TA_Core::Semaphore m_semaphore;

    }; // class ScadaHistorianAgentProcess

} // TA_App

#endif // SCADAHISTORIANAGENTPROCESS_H

